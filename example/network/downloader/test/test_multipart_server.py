#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Simple HTTP server that returns a multipart/form-data response at '/'
Used to test multipart downloads (e.g. j2::network::downloader::multipart_downloader).
"""

import argparse
import io
import mimetypes
import os
import sys
from http.server import HTTPServer, BaseHTTPRequestHandler
from typing import List, Tuple

BOUNDARY_PREFIX = "jaytwo-boundary-"

def build_multipart_body(parts: List[Tuple[str, bytes]]) -> Tuple[bytes, str]:
    """
    parts: list of (filename, bytes)
    returns: (body_bytes, boundary)
    """
    # choose boundary
    boundary = BOUNDARY_PREFIX + os.urandom(8).hex()
    crlf = b"\r\n"
    body = io.BytesIO()

    for filename, content in parts:
        disposition = f'Content-Disposition: form-data; name="file"; filename="{os.path.basename(filename)}"'
        content_type = mimetypes.guess_type(filename)[0] or "application/octet-stream"

        body.write(b"--" + boundary.encode("utf-8") + crlf)
        body.write(disposition.encode("utf-8") + crlf)
        body.write(f"Content-Type: {content_type}".encode("utf-8") + crlf)
        body.write(bcrlf := crlf)  # blank line before content
        body.write(content)
        body.write(crlf)

    # final boundary
    body.write(b"--" + boundary.encode("utf-8") + b"--" + crlf)
    return body.getvalue(), boundary

class MultipartHandler(BaseHTTPRequestHandler):
    server_version = "MultipartTest/1.0"

    def do_GET(self):
        # server holds 'parts' attribute set by server init
        parts = getattr(self.server, "parts", [])

        body, boundary = build_multipart_body(parts)

        self.send_response(200)
        self.send_header("Content-Type", f"multipart/form-data; boundary={boundary}")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()

        # write body in chunks to allow client progress measurement
        CHUNK = 64 * 1024
        sent = 0
        while sent < len(body):
            chunk = body[sent: sent + CHUNK]
            self.wfile.write(chunk)
            sent += len(chunk)

    def log_message(self, format, *args):
        # minimal logging to stderr
        sys.stderr.write("%s - - [%s] %s\n" % (self.client_address[0], self.log_date_time_string(), format % args))

def load_parts_from_files(file_paths: List[str]) -> List[Tuple[str, bytes]]:
    parts = []
    for p in file_paths:
        if os.path.isfile(p):
            with open(p, "rb") as f:
                parts.append((p, f.read()))
        else:
            raise FileNotFoundError(p)
    return parts

def create_sample_parts() -> List[Tuple[str, bytes]]:
    # small text file and larger binary (~1 MiB) to exercise progress reporting
    text = b"Hello from multipart test server!\n"
    bin_content = b"0123456789ABCDEF" * 65536  # ~1,048,576 bytes
    return [("sample.txt", text), ("sample.bin", bin_content)]

def parse_args():
    parser = argparse.ArgumentParser(description="Simple multipart/form-data test HTTP server")
    parser.add_argument("--host", "-H", default="127.0.0.1", help="Host to bind (default: 127.0.0.1)")
    parser.add_argument("--port", "-p", type=int, default=18080, help="Port to listen (default: 18080)")
    parser.add_argument("--files", "-f", nargs="*", help="Files to include in multipart response. If omitted, sample parts are used.")
    return parser.parse_args()

def run_server(host: str, port: int, parts: List[Tuple[str, bytes]]):
    server = HTTPServer((host, port), MultipartHandler)
    # attach parts to server instance so handler can access
    server.parts = parts
    print(f"Serving multipart response on http://{host}:{port}/ with {len(parts)} part(s)")
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("\nServer stopped by user")
    finally:
        server.server_close()

def main():
    args = parse_args()

    if args.files:
        try:
            parts = load_parts_from_files(args.files)
        except FileNotFoundError as e:
            print(f"File not found: {e}", file=sys.stderr)
            sys.exit(2)
    else:
        parts = create_sample_parts()

    run_server(args.host, args.port, parts)

if __name__ == "__main__":
    main()
