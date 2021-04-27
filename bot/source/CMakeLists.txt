cmake_minimum_required(VERSION 3.10)
project(source)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)
include_directories(counters)
include_directories(dbtools)
include_directories(dev/mail_unix)
include_directories(langs)
include_directories(smail)
include_directories(url_dump)

add_executable(source
        counters/bc.c
        counters/bc.h
        counters/bc_dump.c
        counters/bc_dump_f.c
        dbtools/dbtools.cpp
        dbtools/dbtools.h
        dev/mail_unix/sendmail.c
        dev/mail_unix/sendmail.h
        dev/main.cpp
        dict-tools/1-2/update_wtf.c
        dict-tools/update_sowhat.c
        dict-tools/update_wtf.c
        langs/arabic.h
        langs/chinese.h
        langs/dutch.h
        langs/ebonics.h
        langs/english.h
        langs/french.h
        langs/german.h
        langs/greek.h
        langs/italian.h
        langs/lang.h
        langs/latin.h
        langs/latin2.h
        langs/norwegian.h
        langs/portuguese.h
        langs/romanian.h
        langs/russian.h
        langs/russian2.h
        langs/spanish.h
        langs/swedish.h
        langs/taiwanish.h
        libs/1/main.c
        rdb-tools/1.c
        rdb-tools/scan_rdb.cpp
        smail/sendmail.c
        smail/sendmail.h
        url_dump/url_dump.cpp
        url_dump/urlc.h
        bc.c
        bc.h
        bc1.c
        color.c
        color.h
        darkbot.c
        dates.c
        dates.h
        dbtools.cpp
        dbtools.h
        defcodes.h
        defines.h
        langtools.h
        netpack.c
        netpack.h
        seen.cpp
        seen.h
        stdafx.h
        translate.c
        translate.h
        urlc.h
        vars.c
        vera.h
        version.h
        victorina.h)

add_custom_target(
   extern_lib
   COMMAND make
   WORKING_DIRECTORY /projects/linux/olivka-itoolabs/source
)