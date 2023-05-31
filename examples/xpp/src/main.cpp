
#include "xpp_generator.h"
#include "argparse.h"
// @import name

// @doc name

// @macro function(params...)

// @code
// @endcode

// @code(params...)
// @endcode



struct app_args
{
    fst::vector<fst::string> filenames;
    fst::string cache_file;
    fst::string output_file;
    fst::vector<fst::string> includes;
    fst::vector<fst::string> scripts;
};

app_args parse_args(int argc, char* argv[]) {
    app_args args;

    argparse::ArgumentParser program("program_name");
    fst::print(RESOURCES_DIRECTORY "/template");
    program.add_argument("filenames").required().append().help("display the square of a given integer"); //.scan<'i', int>();
    program.add_argument("-c", "--cache").default_value("").help("specify the output file.");
    program.add_argument("-o", "--output").required().help("specify the output file.");
    program.add_argument("-I", "--include").default_value<std::vector<std::string>>({}).append().help("specify the output file.");
    program.add_argument("-S", "--script_dirs").default_value<std::vector<std::string>>({}).append().help("specify the output file.");

    try
    {
        program.parse_args(argc, argv);
    } catch (const std::runtime_error& err)
    {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return args;
    }

    // ./kk "C:/Users/aarsenault/Desktop/dummy/template.h" -I "C:/Users/aarsenault/Desktop/dummy/templates" -S "C:/Users/aarsenault/Desktop/dummy/scripts" -c "C:/Users/aarsenault/Desktop/dummy/build/CMakeCache.txt" -o "C:/Users/aarsenault/Desktop/dummy/output/out.h"
    std::vector<std::string> filenames = program.get<std::vector<std::string>>("filenames"); // "orange"
    args.output_file = fst::string(program.get<std::string>("--output").c_str()); // "orange"
    args.cache_file = fst::string( program.get<std::string>("--cache").c_str()); // "orange"
   
    std::vector<std::string> include_dirs = program.get<std::vector<std::string>>("--include"); // "orange"
    std::vector<std::string> script_dirs = program.get<std::vector<std::string>>("--script_dirs"); // "orange"

    for (const std::string& s : filenames)
    {
        args.filenames.push_back(s.c_str());
    }
    for (const std::string& s : include_dirs)
    {
        args.includes.push_back(s.c_str());
    }

    for (const std::string& s : script_dirs)
    {
        args.scripts.push_back(s.c_str());
       }

    return args;
}

int main(FST_ATTRIBUTE_UNUSED int argc, FST_ATTRIBUTE_UNUSED char* argv[])
{
    //argparse::ArgumentParser program("program_name");
    //fst::print(RESOURCES_DIRECTORY "/template");
    //program.add_argument("filename").required().help("display the square of a given integer");//.scan<'i', int>();
    //program.add_argument("-c", "--cache").default_value("").help("specify the output file.");
    //program.add_argument("-o", "--output").required().help("specify the output file.");
    //program.add_argument("-I", "--include").default_value<std::vector<std::string>>({}).append().help("specify the output file.");
    //program.add_argument("-S", "--script_dirs").default_value<std::vector<std::string>>({}).append().help("specify the output file.");
    //
    //
    //try
    //{
    //    program.parse_args(argc, argv);
    //} catch (const std::runtime_error& err)
    //{
    //    std::cerr << err.what() << std::endl;
    //    std::cerr << program;
    //    return 1;
    //}

    //// ./kk "C:/Users/aarsenault/Desktop/dummy/template.h" -I "C:/Users/aarsenault/Desktop/dummy/templates" -S "C:/Users/aarsenault/Desktop/dummy/scripts" -c "C:/Users/aarsenault/Desktop/dummy/build/CMakeCache.txt" -o "C:/Users/aarsenault/Desktop/dummy/output/out.h"
    //std::string filename = program.get<std::string>("filename"); // "orange"
    //std::string output_file = program.get<std::string>("--output"); // "orange"
    //std::string cache_g = program.get<std::string>("--cache"); // "orange"
    //fst::print(cache_g.c_str());
    //std::vector<std::string> include_dirs = program.get<std::vector<std::string>>("--include"); // "orange"
    //std::vector<std::string> script_dirs = program.get<std::vector<std::string>>("--script_dirs"); // "orange"
    //

    //
    app_args args = parse_args(argc, argv);

    /*fst::string cache_file_path = fst::filesystem::join<fst::string>(args.cache_file, "CMakeCache.txt");
    fst::vector<xpp::cmake_cache_value> cache = dump_cache(cache_file_path.c_str());*/
    //fst::print("KLKLKLKL", fst::filesystem::get_parent_directory( fst::filesystem::get_parent_directory(args.cache_file)));
    //fst::vector<xpp::cmake_cache_value>  cache = dump_cache(RESOURCES_DIRECTORY "/../../../build/CMakeCache.txt");
    
    
    xpp::parser xparser;

//    if (fst::error_result err = xparser.parse_file(RESOURCES_DIRECTORY "/template_01.h"))
        if (fst::error_result err = xparser.parse_file(args.filenames[0].c_str()))
        {
        fst::print("error", err.message());
        return -1;
    }

    //fst::string_view headers[] = { { RESOURCES_DIRECTORY }, { RESOURCES_DIRECTORY "/template" }, { RESOURCES_DIRECTORY "/utils" }, { RESOURCES_DIRECTORY "/traits" } };
    //fst::string_view scripts[] = { { RESOURCES_DIRECTORY }, { RESOURCES_DIRECTORY "/scripts" } };

    fst::vector<fst::string_view> headers;
    fst::vector<fst::string_view> scripts;
    for (const auto& c : args.includes)
    {
        headers.push_back(c.c_str());
    }

    for (const auto& c : args.scripts)
    {
        scripts.push_back(c.c_str());
    }

    xpp::generator xgen;

    if (fst::error_result err = xgen.preprocess(xparser, args.cache_file))
    {
        fst::print("error", err.message());
        return -1;
    }

    if (fst::error_result err = xgen.process(xparser, headers, scripts))
    {
        fst::print("error", err.message());
        return -1;
    }

//    if (fst::error_result err = xgen.generate(RESOURCES_DIRECTORY "/gen.h"))
    //if (fst::error_result err = xgen.generate((args.output_file + "/" + fst::string(fst::filesystem::get_filename(args.filenames[0]))).c_str()))
    if (fst::error_result err = xgen.generate(args.output_file.c_str()))
    {
        fst::print("error", err.message());
        return -1;
    }
    /*const char* fpath = FST_TEST_RESOURCES_DIRECTORY "/script.lua";

    fst::script::engine engine;
    engine.run_string("print('skalskalksaklsa')");
    engine.run_string("prinjkk')");
    engine.run_file(fpath);*/
    return 0;
}

//
//inline std::string_view ltrim(std::string_view s) {
//  std::string_view::size_type pos = s.find_first_not_of(" \n\t\r\v");
//  return pos == std::string_view::npos ? std::string_view() : s.substr(pos);
//}
//
//std::string rtrim(const std::string& s) {
//  std::string::size_type pos = s.find_last_not_of(" \n\t\r\v"); /// must succeed
//  return pos == std::string::npos ? std::string() : s.substr(0, pos + 1);
//}
//
//std::string_view rtrim(std::string_view s) {
//  std::string_view::size_type pos = s.find_last_not_of(" \n\t\r\v"); /// must succeed
//  return pos == std::string_view::npos ? std::string_view() : s.substr(0, pos + 1);
//}
//
//std::string trim(const std::string& s) { return rtrim(ltrim(s)); }
//
//std::string_view trim(std::string_view s) { return rtrim(ltrim(s)); }
