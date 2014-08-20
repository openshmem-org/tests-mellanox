/*
 * Copyright (c) 2014      Mellanox Technologies, Inc.
 *                         All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
#include "osh_def.h"
#include "osh_cmn.h"
#include "osh_log.h"

#if  ( defined(_DEBUG) && defined(WIN32))
#include <crtdbg.h>
#endif


/****************************************************************************
 * Static Function Declarations
 ***************************************************************************/
static int __config_init( void );
static void __config_exit( void );
static int __parse_common_opt( const AOPT_OBJECT* );

static int proc_mode_help( const TE_NODE *, int, const char ** );
static int proc_mode_version( const TE_NODE *, int, const char ** );

extern const TE_NODE exec_tst[];
extern int proc_mode_exec( const TE_NODE *, int, const char ** );


static const TE_NODE osh_modes[] =
{
   { NULL,      proc_mode_help,      "help",         aopt_set_string( "h", "?" ),    "Display list of supported commands."},
   { NULL,      proc_mode_version,   "version",      aopt_set_string( NULL ),        "Output version information and exit."},
   { exec_tst,  proc_mode_exec,      "exec",         aopt_set_string( NULL ),        "Launch defined test."},
   { NULL,      NULL,                NULL,           aopt_set_string( NULL ),        NULL }
};


/*
 * List of supported general options.
 */
const AOPT_DESC  common_opt_desc[] =
{
    {
        'h', AOPT_NOARG,    aopt_set_literal( 'h', '?' ),    aopt_set_string( "help", "usage" ),
             "Show the help message and exit."
    },
    {
        'o', AOPT_ARG,      aopt_set_literal( 'o' ),    aopt_set_string( "out-level" ),
             "Output information level."
    },
    {
        'd', AOPT_ARG,      aopt_set_literal( 'd' ),    aopt_set_string( "dump-level" ),
             "Dump information level."
    },
    {
        'l', AOPT_ARG,      aopt_set_literal( 'l' ),    aopt_set_string( "log-file" ),
             "File name for logging."
    },
    {
        'p', AOPT_ARG,      aopt_set_literal( 0 ),      aopt_set_string( "pe-list" ),
             "Specifiy a numerical list of processors that can output. The list may contain "
             "multiple items, separated by comma, and ranges or 'all'. For example, 0,5,7,9-11"
    },
    { 0, AOPT_NOARG, aopt_set_literal( 0 ), aopt_set_string( NULL ), NULL }
};


/* Common for module variable */
CONFIG osh_config;


int main(int argc, const char *argv[])
{
    OSH_ERROR status = OSH_ERR_NONE;
    const AOPT_OBJECT *common_opt_obj = NULL;

#if  ( defined(_DEBUG) && defined(WIN32) && defined(_MSC_VER))
    /* Set flag to the new value */
   _CrtSetDbgFlag( (_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF) );
#endif
    /* Set initial values in configuration object */
    status = __config_init();

    /* Load supported option and create option objects */
    if ( !status )
    {
        int temp_argc = 0;

        temp_argc = argc;
        common_opt_obj = aopt_init(&temp_argc, (const char **)argv, common_opt_desc);

        /* Set command line common options */
        if (!status && common_opt_obj) 
        {
            status = __parse_common_opt(common_opt_obj);
        }

        /* Destroy option objects */
        aopt_exit((AOPT_OBJECT*)common_opt_obj);
    }

    /* Start logging */
    if ( !status )
    {
        status = log_init();
    }

    /* Launch required mode */
    if ( !status && (argc > 1) ) 
    {
        int i = 0;
        int j = 0;
        int found = 0;

        for (i = 0; osh_modes[i].name != NULL; i++) 
        {
             if (sys_strcmp(osh_modes[i].name, argv[1]) == 0) 
             {
                 found = 1;
             }
             else {
                 for ( j = 0; osh_modes[i].shorts[j] != NULL; j++ ) 
                 {
                     if (sys_strcmp(osh_modes[i].shorts[j], argv[1]) == 0) 
                     {
                         found = 1;
                         break;
                     }
                 }
             }

             if ( found ) 
             {
                 status = osh_modes[i].func(&osh_modes[i], argc - 1, (const char**)(argv + 1));
                 break;
             }
        }

        /*  check if the first option is invalid */
        if (osh_modes[i].name == NULL) 
        {
            status = proc_mode_help(NULL, 0, NULL);
        }
    }
    else 
    {
        status = proc_mode_help(NULL, 0, NULL);
    }

    /* Free configuration object */
    __config_exit();

    return status;
}


/****************************************************************************
 * Static Function Declarations
 ***************************************************************************/

static int proc_mode_help( const TE_NODE *node, int argc, const char **argv )
{
    OSH_ERROR status = OSH_ERR_NONE;
    int   i = 0;
    
    UNREFERENCED_PARAMETER(node);
    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    log_help(MODULE_NAME " is a tool for testing OpenSHMEM.\n");
    log_help("version %s\n", STR(VERSION));
    log_help("\n");
    log_help("Usage: " MODULE_NAME " <subcommand> [options] [args]\n");
    log_help("Type: \'" MODULE_NAME " <subcommand> --help\' for help on a specific subcommand.\n");
    log_help("Type: \'" MODULE_NAME " version\' to see the program version number.\n");
    log_help("\n");
    log_help("Available subcommands:\n");

    for ( i = 0; osh_modes[i].name != NULL; i++ ) {
        char tmp_buf[21];

        /* skip commands with prefix '--', they are special */
        if (osh_modes[i].name[0] != '-') {
            log_help("   %-20.20s\t%-s\n", display_opt(&osh_modes[i], tmp_buf, sizeof(tmp_buf)), osh_modes[i].note);
        }
    }

    log_help("\n");
    log_help("For additional information see README file, or Type \'" MODULE_NAME " <subcommand> --help\'.\n");

    return status;
}


static int proc_mode_version( const TE_NODE *node, int argc, const char **argv )
{
    OSH_ERROR status = OSH_ERR_NONE;

    UNREFERENCED_PARAMETER(node);
    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    log_help(MODULE_NAME ", version %s\n", STR(VERSION));
    log_help("   compiled %s, %s\n", __DATE__, __TIME__);
    log_help("\n%s\n", MODULE_COPYRIGHT);

    return status;
}


static int __config_init( void )
{
    OSH_ERROR status = OSH_ERR_NONE;
    char temp_buf[1024];
    static int global_test_rc = 0;

    sys_memset(&osh_config, 0, sizeof(osh_config));

    osh_config.active = 0;
    if (gethostname(osh_config.hostname, sizeof(osh_config.hostname)) != 0)
    {
        sys_strcpy(osh_config.hostname, "localhost");
    }
    osh_config.my_pe = INVALID_PE;
    osh_config.num_pe = INVALID_PE;
    osh_config.rc = &global_test_rc; /*NULL*/
    /* 
     * Seed the random-number generator with current time so that
     * the numbers will be different every time we run.
     */
    osh_config.timer = (unsigned int)time( NULL );
    srand( osh_config.timer );

    PE_LIST_SET(DEFAULT_PE, osh_config.exec_mode.log_pe_list);
    osh_config.exec_mode.out_level = LOG_INFO;
    osh_config.exec_mode.out_file = stdout;

    osh_config.exec_mode.log_level = LOG_NONE;
    osh_config.exec_mode.log_file = NULL;
    sys_snprintf_safe(temp_buf, sizeof(temp_buf), "%s_%s_%04d.log", MODULE_NAME, osh_config.hostname, getpid());
    osh_config.exec_mode.log_file_name = sys_strdup(temp_buf);

    return status;
}


static void __config_exit( void )
{
    if (osh_config.exec_mode.log_file_name)
    {
        sys_free(osh_config.exec_mode.log_file_name);
    }
}


static int __parse_common_opt( const AOPT_OBJECT* opt_obj )
{
    OSH_ERROR status = OSH_ERR_NONE;

    if (opt_obj) 
    {
        if ( !status && aopt_check(opt_obj, 'o') ) 
        {
            const char* optarg = aopt_value(opt_obj, 'o');
            long value = -1;
            if (optarg) 
            {
                errno = 0;
                value = sys_strtol(optarg, NULL, 0);
                if ( !errno && (value >= LOG_NONE) ) 
                {
                    osh_config.exec_mode.out_level = (int)value;
                }
            }
            if ( value == -1 ) 
            {
                status = OSH_ERR_BAD_ARGUMENT;
            }
        }

        if ( !status && aopt_check(opt_obj, 'd') ) 
        {
            const char* optarg = aopt_value(opt_obj, 'd');
            long value = -1;
            if (optarg) 
            {
                errno = 0;
                value = sys_strtol(optarg, NULL, 0);
                if ( !errno && (value >= LOG_NONE) ) 
                {
                    osh_config.exec_mode.log_level = (int)value;
                }
            }
            if ( value == -1 ) 
            {
                status = OSH_ERR_BAD_ARGUMENT;
            }
        }

        if ( !status && aopt_check(opt_obj, 'l') ) 
        {
            const char * optarg = aopt_value(opt_obj, 'l');
            char* value = NULL;
            if (optarg) 
            {
                value = sys_strdup(optarg);
                if ( value )
                {
                    if (osh_config.exec_mode.log_file_name)
                    {
                        sys_free(osh_config.exec_mode.log_file_name);
                    }
                    osh_config.exec_mode.log_file_name = value;
                }
            }
            if ( !value )
            {
                status = OSH_ERR_BAD_ARGUMENT;
            }
        }

        if ( !status && aopt_check(opt_obj, 'p') ) 
        {
            const char* optarg = aopt_value(opt_obj, 'p');
            unsigned long long value = 0;
            if (optarg) 
            {
                if (!sys_strcmp(optarg, "all"))
                {
                    value = INVALID_PE;
                }
                else
                {
                    value = set_pe_list(optarg);
                }
                osh_config.exec_mode.log_pe_list = value;
            }
            if ( !value ) 
            {
                status = OSH_ERR_BAD_ARGUMENT;
            }
        }
    }

    return status;
}
