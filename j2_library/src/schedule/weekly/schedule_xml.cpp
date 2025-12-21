#include "j2_library/schedule/weekly/schedule_xml.hpp"

#include <tinyxml2.h>
#include <string>

namespace j2::schedule::weekly {

    std::string to_xml(const weekly_ranges& ranges) {
        tinyxml2::XMLPrinter printer;
        static const char* day_name[] = { "Mon","Tue","Wed","Thu","Fri","Sat","Sun" };

        // XML 선언 추가: <?xml version="1.0" encoding="UTF-8"?>
        printer.PushDeclaration("xml version=\"1.0\" encoding=\"UTF-8\"");

        printer.OpenElement("WeeklySchedule");

        for (const auto& r : ranges) {
            printer.OpenElement("Range");
            printer.PushAttribute("start_day", day_name[static_cast<int>(r.start_day)]);
            printer.PushAttribute("start_h", r.start_time.hour);
            printer.PushAttribute("start_m", r.start_time.minute);
            printer.PushAttribute("end_day", day_name[static_cast<int>(r.end_day)]);
            printer.PushAttribute("end_h", r.end_time.hour);
            printer.PushAttribute("end_m", r.end_time.minute);
            printer.CloseElement(); // Range
        }

        printer.CloseElement(); // WeeklySchedule
        return std::string(printer.CStr());
    }

}
