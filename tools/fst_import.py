#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os;
import platform;
import argparse;
import sys;

# ./tools/fst_import.py '/c/Users/aarsenault/dev/fst/dev/include/fst/template_test.h' -o '/c/Users/aarsenault/dev/fst/build/sources/include/fst' -I '/c/Users/aarsenault/dev/fst/dev/template'

    # dir_path = os.path.dirname(os.path.realpath(__file__));
    # project_directory = os.path.join(dir_path, "..");    
    # include_directory = os.path.join(project_directory, "dev", "template");


import_key = "@import";
doc_key = "@doc";
macro_key = "@macro";
script_begin_key = "@code";
script_end_key = "@endcode";


command_tokens = [import_key, doc_key, script_begin_key, script_end_key, macro_key];

def find_imported_file(fpath, includes):
    for dir in includes:
        path = os.path.join(dir, fpath);
        if os.path.isfile(path):
            return os.path.abspath(path);
    return None;

class Command:
    def __init__(self, start, end, cmd, v, ex):
        self.start_index = start;
        self.end_index = end;
        self.command = cmd;
        self.value = v;
        self.extra = ex;

    def is_valid(self):
        return self.start_index != -1;

#
#
def find_next_key(file_content, start = 0):
    keyword_index = -1;
    keyword_cmd = None;

    for token in command_tokens:
        kindex = file_content.find(token, start);

        if kindex != -1 and (keyword_index == -1 or kindex < keyword_index):
            keyword_index = kindex;
            keyword_cmd = token;

    if keyword_index != -1:
        after_keyword_index = keyword_index + len(keyword_cmd);
        line_end_index = file_content.find("\n", after_keyword_index);
        if line_end_index == -1:
            line_end_index = after_keyword_index;
        
        cmd_extra = None
        cmd_value = None
        # cmd_value = file_content[after_keyword_index : line_end_index];#.strip();
        if keyword_cmd == import_key:
            cmd_value = file_content[after_keyword_index : line_end_index];#.strip();

        if keyword_cmd == script_begin_key:
            cmd_value = file_content[after_keyword_index : line_end_index].strip();
            jkjks = file_content.find("(", after_keyword_index);
            if jkjks != -1:
                ddd = file_content.find(")", jkjks);
                if ddd != -1:
                    cmd_extra = file_content[jkjks + 1 : ddd].strip();
        
        elif keyword_cmd == macro_key:
            cmd_value = file_content[after_keyword_index : line_end_index].strip();
            jkjks = file_content.find("(", after_keyword_index);
            if jkjks != -1:
                cmd_value = file_content[after_keyword_index : jkjks].strip();
                ddd = file_content.find(")", jkjks);
                if ddd != -1:
                    cmd_extra = file_content[jkjks + 1 : ddd].strip();
            
        print('xxxxxx', keyword_index, line_end_index, keyword_cmd, cmd_value, cmd_extra)
        return Command(keyword_index, line_end_index, keyword_cmd, cmd_value, cmd_extra);

    print('xxxxxddx')

    return Command(-1, -1, "", "", None);


class fstapi:
    def __init__(self, content, filepath, params):
        self.content = content;
        self.filepath = filepath;
        self.params = params;

    def create_enum(self, a):
        # print("DSDSDS")
        return "enum class " + a + " {};"
            
    
    def parse_enum(self, a, fcontent):
        # print("DSDSDS")
        k = fcontent.find("enum class " + a);
        if k != -1:
            kbegin = fcontent.find("{", k);
            kend = fcontent.find("}", kbegin);
    
            ddda = fcontent[kbegin + 1 : kend];
            ddda = ddda.replace('\n', '').strip();
            ddda = [x.strip() for x in ddda.split(',')]
            # print(ddda)
            cc = 'const char* enum_array[] = {';

            alen = len(ddda)
            for count, item in enumerate(ddda):
                cc = cc + '"' + item + '"';
                if count != alen - 1:
                    cc = cc + ', ';
            # for x in ddda:
                # cc = cc + '"' + x + '"' + ', ';

            cc = cc + '};'
            return cc;#'const char* array[] = {' + '{0}'.format(ddda) + '};'

def parse_file(input_file_path, include_dirs):

    try:
        file_content = open(input_file_path, "r+").read();
    except:
        sys.exit("could not open input file", input_file_path)

    cmd = find_next_key(file_content);

    while cmd.is_valid():
        
        if cmd.command == import_key:
            print('-0-0-0-0-0 ', cmd.value + ".template")
            imported_filepath = find_imported_file(cmd.value.strip() + ".template", include_dirs);

            if type(imported_filepath) == str:

                try:
                    template_file = open(imported_filepath, "r+");
                    template_file_content = template_file.read();
                    template_file.close();
                except:
                    print('Could not open imported file', imported_filepath);
                    sys.exit("some error message");

                full_line = file_content[cmd.start_index : cmd.end_index + 1];
                file_content = file_content.replace(full_line, template_file_content + '\n');
                cmd.end_index = cmd.start_index + len(template_file_content);
                # template_file_content.close();

            else:
                print("Could not find import file", cmd.value);

        elif cmd.command == macro_key:
            print("MACRO", cmd.value)
            print("MACRO", cmd.extra)

        elif cmd.command == doc_key:
            print("DSLKJDS", cmd.value)

        elif cmd.command == script_begin_key:
            # print("sccc", cmd.value)
            begin_cmd = cmd;

            cmd = find_next_key(file_content, cmd.end_index);

            if not cmd.is_valid():
                print('Could not open imported file', imported_filepath);
                sys.exit("some error message");
        
            if cmd.command != script_end_key:
                print('Could not open imported file', imported_filepath);
                sys.exit("some error message");
        
            # sc_content = file_content[begin_cmd.end_index + 1 : cmd.start_index - 1]
            sc_content = file_content[begin_cmd.end_index : cmd.start_index]
            # print("---------", sc_content, "---")
            sc_lines = sc_content.splitlines();
            lines = [];

            for l in sc_lines:
                lines.append(l.removeprefix(" ")) 

                # lines.append(l.removeprefix("//").removeprefix(" ")) 

            ssss = '\n'.join(lines)
            extra = None;
            
            
                # return "enum class " + a + " {};"

            # symbols = {'content': file_content, 'output':None, 'filepath':input_file_path, 'create_enum':create_enum, 'parse_enum':parse_enum};
    # def __init__(self, content, filepath, params):
            # print(ssss)
            if begin_cmd.extra is not None:
                extra = [x.strip() for x in begin_cmd.extra.split(',')]

                # extra = begin_cmd.extra.split(',');
                # print(extra)
                # symbols['params'] = extra;
            aaaaa = fstapi(file_content, input_file_path, extra);
            symbols = {'fst':aaaaa, 'output':None}
            # context = {'context' :symbols};
            # symbols = {'jb' :jb}
            # eval(repr(ssss), globals(), symbols)

            # locals()['content'] = None;
            output = ""
            # ssss = repr(ssss)
            # ssss = ssss.replace('\n', '\n\t')
            code = ssss.splitlines();
            code_lines = []
            for l in code:
                code_lines.append('\t' + l);
            ssss = '\n'.join(code_lines);
               
            ssss = "def call_fct(fst):\n\tparams = fst.params;\n" + ssss + "\noutput = call_fct(fst);";
            # print(".....................\n",ssss)

            # all = {'output': None,'all':symbols}
            # ff =
            exec(ssss, globals(), symbols)

            # symbols['output'](symbols['args']);
            # rrrr = ff(symbols['args'])
            # print(locals()['jb'])
            # print("kkkkkkkkkkkkkkk", symbols['content'])
            
            # if symbols['content'] is None:
            if type(symbols['output']) == str:
                output = symbols['output'];
                code = output.splitlines();
                code_lines = []
                for l in code:
                    code_lines.append(l.removeprefix('\t'));
                output = '\n'.join(code_lines);

                end_index = cmd.end_index;
                replace_content = file_content[begin_cmd.start_index : end_index];
                file_content = file_content.replace(replace_content, output);
                cmd.end_index = begin_cmd.start_index;
            else:
                end_index = min(len(file_content), cmd.end_index + 2);
                replace_content = file_content[begin_cmd.start_index : end_index];
                file_content = file_content.replace(replace_content, output);
                cmd.end_index = begin_cmd.start_index;

        elif cmd.command == script_end_key:
            print("dsadsadsadsdsa", cmd.value)

        # Next.
        cmd = find_next_key(file_content, cmd.end_index);

    return file_content;


if __name__ == "__main__":

    parser = argparse.ArgumentParser(
                    prog='ProgramName',
                    description='What the program does',
                    epilog='Text at the bottom of help')

    parser.add_argument('filename');
    parser.add_argument('-o', '--output_dir', required=True, dest='output', help="platform");
    parser.add_argument('-I', '--include', dest='include_dirs', metavar='N', nargs='+', help='include directories');
    
    args = parser.parse_args();
    input_file_path = args.filename;
    output_file_path = args.output;
    include_dirs = args.include_dirs;

    # 
    file_content = parse_file(input_file_path, include_dirs);

    # 
    output_file_name = os.path.join(output_file_path, os.path.basename(input_file_path));
    try:
        output_file = open(output_file_name, "w")
        output_file.write(file_content)
        output_file.close()
    except:
        print('Could not open output file', output_file_name);
        sys.exit("some error message");
