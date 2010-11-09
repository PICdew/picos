#ifndef ARGS_PARSER
#define ARGS_PARSER

#include <getopt.h>
static char doc[] = "assembler -- compiles source code for piclang, a functional language for PIC microcontrollers.";
static char usage_doc[] = "assembler <options>";
struct good_option{
	struct option getopts_options;
	std::string help_string,arg_type;
};

static struct good_option options[] = {
  {{"todo",no_argument,0,0},"Prints a list of things that need to be done to improve this program.",""},
  {{"version",no_argument,0,1},"Printer the current version",""},
  {{"compile",required_argument,0,'c'},"Compiles a specific file.","FILE"},
  {{"output",required_argument,0,'o'},"Name of the output file (HEX format).","FILE"},
  {{0,0,0,0},"",""}  
};

struct assembler_arguments
{
  std::string source_filename,output_filename;  
};

void args_usage()
{
	using namespace std;
	struct good_option curr_opt;
	size_t opt_index = 0;
	cout << doc << endl;
	cout << "Usage: " << usage_doc << endl;
	curr_opt = options[opt_index++];
	
	cout << endl;
	while(curr_opt.getopts_options.name != 0)
	{
		struct option& gnu_opt = curr_opt.getopts_options;
		cout << "--" << gnu_opt.name;
		if(gnu_opt.flag != 0)
			cout << ", -" << (char) *gnu_opt.flag;
		if(gnu_opt.has_arg == required_argument)cout << "[";
		if(gnu_opt.has_arg == optional_argument)cout << "<";
		if(gnu_opt.has_arg != no_argument)cout << curr_opt.arg_type;
		if(gnu_opt.has_arg == required_argument)cout << "]";
		if(gnu_opt.has_arg == optional_argument)cout << ">";
		
		cout << " : " << curr_opt.help_string;
		cout << endl;
		curr_opt = options[opt_index++];
	}
	exit(0);
}

enum ARG_ERROR{ARGS_UNKOWN = 1};
static ARG_ERROR parse_opt(int key, struct good_option* the_options, int index,struct assembler_arguments& args)
{
	using namespace std;
	struct good_option option = the_options[index];

	switch(key)
	{
	case 't':
	  cout << Build::todo << endl;
	  args_usage();
	case 'v':
	  cout << "assembler version: " << Build::getVersion() << endl;
	  cout << "opcode version: " << opcodeVersion[0] << "." << opcodeVersion[1] << "." << opcodeVersion[2] << endl;
	  cout << "Last Build: " << Build::getBuild() << endl;
	  args_usage();
	case 'c':
	  args.source_filename =  optarg;
	  break;
	case 'o':
	  args.output_filename = optarg;
	  break;
	default:
	  return ARGS_UNKNOWN;
	}
	return 0;
}


void parse_args(int argc, char** argv, struct assembler_arguments& args)
{
	int c,option_index,opt_result;
	
	while(true)
	{
		c = getopt_long(argc,argv,"tvc:o:",options,option_index);
		if(c == -1)
			break;//end of args
		parse_opts(c,&options,option_index,args);
	}
}

#endif