#!/bin/bash


#-- Output usage message --#
function usage()
{
    echo "Usage: $0 [-p <parent_path>] [-t [lua | c++]] -s <service_name>";
    echo "     : Create the cfashttp service template tool";
    echo "";
    echo "Options: ";
    echo "    -p Specify the parent path name";
    echo "    -s Specify the service name";
    echo "    -t Specify the c or c++, default service type c";
    echo "";
    echo "Version: 0.0.1 (bash)"
    echo "Message: Thanks!";

    exit 0;
}


#-- Create c++ service template --#
function create_cxx_service_tpl()
{
    echo "--------------------------------------------------------------------------";
    service_path=$1"/"$2;
    service_file=$service_path/$2".cxx";
    echo "[Create ]: The service.<$2> type of c++ language";
    echo "[Create ]: The pathname.<$service_path>";
    if [ -x $service_path ]; then
        echo "[Existed]: The path name.<$service_path> is existed";
    else
        mkdir $service_path;
    fi

    echo "[Create ]: Create the service.<$2> c++ file...";

    touch $service_file;

    #-- Init function --#
    echo '#include "core/eris.core.h"' >  $service_file;
    echo '#include "eris/eris.version.h"' >>  $service_file;
    echo ""  >> $service_file;
    echo ""  >> $service_file;
    echo "extern \"C\" eris_int_t $2_service( const eris_module_t *__mcontext, eris_http_t *__http, eris_log_t *__log)" >> $service_file;
    echo "{" >> $service_file;
    echo '    //eris_log_dump( __log, ERIS_LOG_NOTICE, "%s", __func__);' >> $service_file;
    echo ""  >> $service_file;
    echo "    eris_int_t rc = 0;"  >> $service_file;
    echo ""  >> $service_file;
    echo ""  >> $service_file;
    echo "    /** Set status */"  >> $service_file;
    echo '    eris_http_response_set_status( __http, ERIS_HTTP_200);'  >> $service_file;
    echo ""  >> $service_file;
    echo "    /** Set Content-Type */"  >> $service_file;
    echo '    eris_http_response_set_header( __http, "Content-Type", "text/plain");'  >> $service_file;
    echo ""  >> $service_file;
    echo "    /** Set body data */"  >> $service_file;
    echo '    rc = eris_http_response_set_body( __http, (const eris_void_t *)"Erishttp Service", 16);' >> $service_file;
    echo ""  >> $service_file;
    echo ""  >> $service_file;
    echo "    return rc;" >> $service_file;
    echo "}" >> $service_file;
    echo ""  >> $service_file;
    echo ""  >> $service_file;
    echo "[Create ]: service.<$2> ok!";
    echo "--------------------------------------------------------------------------";
}


#-- Create the c++ service makefile --#
function create_cxx_service_mkf()
{
    echo "--------------------------------------------------------------------------";
    service_path=$1"/"$2;
    service_makefile=$service_path/"Makefile";
    echo "[Create ]: The service makefile";

    #-- Create makefile --#
    touch $service_makefile;
    echo "CXX=g++"  > $service_makefile;
    echo "CXXFLAGS=-std=c++0x -g -Wall" >> $service_makefile;
    echo "" >> $service_file;
    echo ".PHONY: all clean" >> $service_makefile;
    echo "target=lib$2.so.0.0.1" >> $service_makefile;
    echo "sources=$2.cxx" >> $service_makefile;
    echo "" >> $service_file;
    echo "objects=$2.o" >> $service_makefile;
    echo "" >> $service_makefile;
    echo "" >> $service_makefile;
    echo "inc_opts =-I../../include" >> $service_makefile;
    echo "" >> $service_makefile;
    echo "lib_opts =-L../../lib" >> $service_makefile;
    echo "" >> $service_makefile;
    echo "libs =-lerishttp" >> $service_makefile;
    echo "" >> $service_makefile;
    echo 'all:$(target)' >> $service_makefile;
    echo '$(target): $(objects)' >> $service_makefile;
    echo '	$(CXX) $(CXXFLAGS) -shared $^ -o $@ -L../../lib $(lib_opts) $(libs)' >> $service_makefile;
    echo "" >> $service_makefile;
    echo "" >> $service_makefile;
    echo '$(objects):%.o:%.cxx' >> $service_makefile;
    echo '	$(CXX) $(CXXFLAGS) -fPIC $(inc_opts) -c $^ -o $@' >> $service_makefile;
    echo "" >> $service_makefile;
    echo "" >> $service_makefile;
    echo "clean:" >> $service_makefile;
    echo '	@rm $(target) $(objects)' >> $service_makefile;
    echo "" >> $service_makefile;
    echo "" >> $service_makefile;
    echo "[Create ]: service.<$2> makefile ok!";
    echo "--------------------------------------------------------------------------";
}


#-- Create c++ service template --#
function create_c_service_tpl()
{
    echo "--------------------------------------------------------------------------";
    service_path=$1"/"$2;
    service_file=$service_path/$2".c";
    echo "[Create ]: The service.<$2> type of c langugae";
    echo "[Create ]: The pathname.<$service_path>";
    if [ -x $service_path ]; then
        echo "[Existed]: The path name.<$service_path> is existed";
    else
        mkdir $service_path;
    fi

    echo "[Create ]: Create the service.<$2> c++ file...";

    touch $service_file;

    #-- Init function --#
    echo '#include "core/eris.core.h"' >  $service_file;
    echo '#include "eris/eris.version.h"' >>  $service_file;
    echo ""  >> $service_file;
    echo ""  >> $service_file;
    echo "eris_int_t $2_service( const eris_module_t *__mcontext, eris_http_t *__http, eris_log_t *__log)" >> $service_file;
    echo "{" >> $service_file;
    echo '    //eris_log_dump( __log, ERIS_LOG_NOTICE, "%s", __func__);' >> $service_file;
    echo ""  >> $service_file;
    echo "    eris_int_t rc = 0;"  >> $service_file;
    echo ""  >> $service_file;
    echo ""  >> $service_file;
    echo "    /** Set status */"  >> $service_file;
    echo '    eris_http_response_set_status( __http, ERIS_HTTP_200);'  >> $service_file;
    echo ""  >> $service_file;
    echo "    /** Set Content-Type */"  >> $service_file;
    echo '    eris_http_response_set_header( __http, "Content-Type", "text/plain");'  >> $service_file;
    echo ""  >> $service_file;
    echo "    /** Set body data */"  >> $service_file;
    echo '    rc = eris_http_response_set_body( __http, (const eris_void_t *)"Erishttp Service", 16);' >> $service_file;
    echo ""  >> $service_file;
    echo ""  >> $service_file;
    echo "    return rc;" >> $service_file;
    echo "}" >> $service_file;
    echo ""  >> $service_file;
    echo ""  >> $service_file;
    echo "[Create ]: service.<$2> ok!";
    echo "--------------------------------------------------------------------------";
}


#-- Create the c service makefile --#
function create_c_service_mkf()
{
    echo "--------------------------------------------------------------------------";
    service_path=$1"/"$2;
    service_makefile=$service_path/"Makefile";
    echo "[Create ]: The service makefile";

    #-- Create makefile --#
    touch $service_makefile;
    echo "CXX=gcc"  > $service_makefile;
    echo "CXXFLAGS=-g -Wall" >> $service_makefile;
    echo "" >> $service_file;
    echo ".PHONY: all clean" >> $service_makefile;
    echo "target=lib$2.so.0.0.1" >> $service_makefile;
    echo "sources=$2.c" >> $service_makefile;
    echo "" >> $service_file;
    echo "objects=$2.o" >> $service_makefile;
    echo "" >> $service_makefile;
    echo "" >> $service_makefile;
    echo "inc_opts =-I../../include" >> $service_makefile;
    echo "" >> $service_makefile;
    echo "lib_opts =-L../../lib" >> $service_makefile;
    echo "" >> $service_makefile;
    echo "libs =-lerishttp" >> $service_makefile;
    echo "" >> $service_makefile;
    echo 'all:$(target)' >> $service_makefile;
    echo '$(target): $(objects)' >> $service_makefile;
    echo '	$(CXX) $(CXXFLAGS) -shared $^ -o $@ -L../../lib $(lib_opts) $(libs)' >> $service_makefile;
    echo "" >> $service_makefile;
    echo "" >> $service_makefile;
    echo '$(objects):%.o:%.c' >> $service_makefile;
    echo '	$(CXX) $(CXXFLAGS) -fPIC $(inc_opts) -c $^ -o $@' >> $service_makefile;
    echo "" >> $service_makefile;
    echo "" >> $service_makefile;
    echo "clean:" >> $service_makefile;
    echo '	@rm $(target) $(objects)' >> $service_makefile;
    echo "" >> $service_makefile;
    echo "" >> $service_makefile;
    echo "[Create ]: service.<$2> makefile ok!";
    echo "--------------------------------------------------------------------------";
}


if [ $# -ne 0 ]; then
    parent_path=".";
    service_type="c";
    service_name=;
    while getopts "p:t:s:h" opt_c; do
        case ${opt_c} in
            p)
                parent_path=$OPTARG;
            ;;
            t)
                service_type=$OPTARG;
            ;;
            s)
                service_name=$OPTARG;
            ;;
            h)
                #-- Output message --#
                usage $0;
            ;;
            ?)
                #-- Output message --#
                usage $0;
            ;;
        esac
    done

    #-- Output message --#
    if [ -f "$parent_path/$service_name/$service_name.c" ]; then
        echo "[Notice]: Service <$service_name> c langugage type is existed.";

    elif [ -f "$parent_path/$service_name/$service_name.cxx" ]; then
        echo "[Notice]: Service <$service_name> c++ langugage type is existed.";

    else 
        if [ -n $service_name ]; then
            if [ "$service_type" == "c++" ] || [ "$service_type" == "C++" ]; then
                create_cxx_service_tpl $parent_path $service_name;
                create_cxx_service_mkf $parent_path $service_name;

            elif [ "$service_type" == "c" ] || [ "$service_type" == "C" ]; then
                create_c_service_tpl $parent_path $service_name;
                create_c_service_mkf $parent_path $service_name;

            else 
                echo "[Error]: Unknow service type.<$service_type>";
            fi
        else
            usage $0;
        fi
    fi
else
    #-- Output message --#
    usage $0;
fi


