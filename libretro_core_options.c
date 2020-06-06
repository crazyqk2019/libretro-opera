#include <libretro_core_options.h>
#include <libretro.h>
#include <retro_miscellaneous.h>

#include "retro_callbacks.h"
#include "libopera/opera_bios.h"

#include <file/file_path.h>

#include <stdlib.h>
#include <string.h>

/*
********************************
* Core Option Definitions
********************************
*/

/* RETRO_LANGUAGE_ENGLISH */

/* Default language:
 * - All other languages must include the same keys and values
 * - Will be used as a fallback in the event that frontend language
 *   is not available
 * - Will be used as a fallback for any missing entries in
 *   frontend language definition */

static struct retro_core_option_definition option_defs_us[] =
  {
    {
      "opera_bios",
      "BIOS (rom1)",
      "Select which official system BIOS (hardware revision) to use when performing emulation. Only files present in RetroArch's system directory are listed.",
      {
        { NULL, NULL }, /* This is set dynamically */
      },
      NULL
    },
    {
      "opera_font",
      "Font (rom2)",
      "Select which official 'font rom' to use when performing emulation. Required by some Japanese games, otherwise optional. Only files present in RetroArch's system directory are listed.",
      {
        { NULL, NULL }, /* This is set dynamically */
      },
      NULL
    },
    {
      "opera_cpu_overclock",
      "CPU Overclock",
      "Increase clock speed of the emulated 3DO's 12.5MHz ARM60 CPU. Dramatically improves frame rates in many games (e.g. The Need For Speed), but increases performance requirements and in some cases has no effect (or may cause glitches).",
      {
        { "1.0x (12.50Mhz)", NULL },
        { "1.1x (13.75Mhz)", NULL },
        { "1.2x (15.00Mhz)", NULL },
        { "1.5x (18.75Mhz)", NULL },
        { "1.6x (20.00Mhz)", NULL },
        { "1.8x (22.50Mhz)", NULL },
        { "2.0x (25.00Mhz)", NULL },
        { NULL, NULL },
      },
      "1.0x (12.50Mhz)"
    },
    {
      "opera_region",
      "Mode",
      "Select the resolution and field rate. NOTE: some EU games require a EU ROM.",
      {
        { "ntsc", "NTSC 320x240@60" },
        { "pal1", "PAL1 320x288@50" },
        { "pal2", "PAL2 352x288@50" },
        { NULL, NULL }
      },
      "ntsc"
    },
    {
      "opera_vdlp_pixel_format",
      "VDLP Pixel Format",
      "Select the pixel format to request from the runtime and convert to from the internal 16bpp format.",
      {
        { "0RGB1555", NULL },
        { "RGB565",   NULL },
        { "XRGB8888", NULL }
      },
      "XRGB8888"
    },
    {
      "opera_vdlp_bypass_clut",
      "VDLP Bypass CLUT",
      "Force 3DO VDLP to bypass the game's CLUT (color lookup table). May result in incorrect colors but faster rendering.",
      {
        { "disabled", NULL },
        { "enabled",  NULL },
        { NULL, NULL }
      },
      "disabled"
    },
    {
      "opera_high_resolution",
      "HiRes CEL Rendering",
      "Enabling this option causes CELs to be rendered at 2x the resolution into a 2x output framebuffer, increasing the fidelity of 3D models. Will not affect 2D games. Has a significant performance impact.",
      {
        { "disabled", NULL },
        { "enabled",  NULL },
        { NULL, NULL },
      },
      "disabled"
    },
    {
      "opera_madam_matrix_engine",
      "MADAM Matrix Engine",
      "'MADAM' is the 3DO's graphics accelerator. It contains a custom maths co-processor, used by the 3DO's CPU to offload matrix operations. This corresponds to running in 'Hardware' - but the 3DO could also utilise a built-in ARM 'Software' version of the MADAM matrix engine. 'Hardware' mode is the default, but it has been observed that some games run faster when forcing 'Software' mode (e.g. The Need For Speed).",
      {
        { "hardware", "Hardware" },
        { "software", "Software" },
        { NULL, NULL },
      },
      "hardware"
    },
    {
      "opera_swi_hle",
      "OperaOS SWI HLE",
      "3DO games are built on top of Portfolio OS. Opera has high level emulation of certain OS functions. This HLE may improve performance in on certain games.",
      {
        { "disabled", NULL },
        { "enabled",  NULL },
        { NULL, NULL }
      },
      "disabled"
    },
#if THREADED_DSP
    {
      "opera_dsp_threaded",
      "Threaded DSP",
      "Run the DSP (audio processor) on a separate CPU thread. Improves performance on multi-core systems. !EXPERIMENTAL!",
      {
        { "disabled", NULL },
        { "enabled",  NULL },
        { NULL, NULL },
      },
      "disabled"
    },
#endif
    {
      "opera_nvram_storage",
      "NVRAM Storage",
      "Selects whether NVRAM saves should be created per-game or as a single NVRAM file shared between all games.",
      {
        { "per game", "Per Game" },
        { "shared",   "Shared" },
        { NULL, NULL },
      },
      "per game"
    },
    {
      "opera_active_devices",
      "Active Input Devices",
      "There exists a bug (perhaps in Opera itself, but possibly in certain games) where having more than 1 emulated controller causes content to ignore gamepad input. Setting 'Active Input Devices' to 1 provides a workaround for this issue.",
      {
        { "0", NULL },
        { "1", NULL },
        { "2", NULL },
        { "3", NULL },
        { "4", NULL },
        { "5", NULL },
        { "6", NULL },
        { "7", NULL },
        { "8", NULL },
        { NULL, NULL },
      },
      "1"
    },
    {
      "opera_hack_timing_1",
      "Timing Hack 1 (Crash 'n Burn)",
      "This must be enabled for correct operation of the game 'Crash 'n Burn'.",
      {
        { "disabled", NULL },
        { "enabled",  NULL },
        { NULL, NULL },
      },
      "disabled"
    },
    {
      "opera_hack_timing_3",
      "Timing Hack 3 (Dinopark Tycoon)",
      "This must be enabled for correct operation of the game 'Dinopark Tycoon'.",
      {
        { "disabled", NULL },
        { "enabled",  NULL },
        { NULL, NULL },
      },
      "disabled"
    },
    {
      "opera_hack_timing_5",
      "Timing Hack 5 (Microcosm)",
      "This must be enabled for correct operation of the game 'Microcosm'.",
      {
        { "disabled", NULL },
        { "enabled",  NULL },
        { NULL, NULL },
      },
      "disabled"
    },
    {
      "opera_hack_timing_6",
      "Timing Hack 6 (Alone in the Dark)",
      "This must be enabled for correct operation of the game 'Alone in the Dark'.",
      {
        { "disabled", NULL },
        { "enabled",  NULL },
        { NULL, NULL },
      },
      "disabled"
    },
    {
      "opera_hack_graphics_step_y",
      "Graphics Step Y Hack (Samurai Showdown)",
      "This must be enabled for backgrounds to render correctly when running the game 'Samurai Showdown'.",
      {
        { "disabled", NULL },
        { "enabled",  NULL },
        { NULL, NULL },
      },
      "disabled"
    },
    {
      "opera_kprint",
      "Debug Output",
      "Print 3DO debug port output to stderr. Only really useful to 3DO homebrew developers.",
      {
        { "disabled", NULL },
        { "enabled",  NULL },
        { NULL, NULL },
      },
      "disabled"
    },
    { NULL, NULL, NULL, {{0}}, NULL },
  };

/* RETRO_LANGUAGE_JAPANESE */

/* RETRO_LANGUAGE_FRENCH */

/* RETRO_LANGUAGE_SPANISH */

/* RETRO_LANGUAGE_GERMAN */

/* RETRO_LANGUAGE_ITALIAN */

/* RETRO_LANGUAGE_DUTCH */

/* RETRO_LANGUAGE_PORTUGUESE_BRAZIL */

/* RETRO_LANGUAGE_PORTUGUESE_PORTUGAL */

/* RETRO_LANGUAGE_RUSSIAN */

/* RETRO_LANGUAGE_KOREAN */

/* RETRO_LANGUAGE_CHINESE_TRADITIONAL */

/* RETRO_LANGUAGE_CHINESE_SIMPLIFIED */
static struct retro_core_option_definition option_defs_chs[] =
  {
    {
      "opera_bios",
      "BIOS (rom1)",
      "选择要使用的官方系统BIOS。只列出在RetroArch系统目录下存在的文件。",
      {
        { NULL, NULL }, /* This is set dynamically */
      },
      NULL
    },
    {
      "opera_font",
      "字体 (rom2)",
      "选择要使用的官方字体ROM。某些日版游戏需要，其它游戏可选。\n"
      "只列出在RetroArch系统目录下存在的文件。",
      {
        { NULL, NULL }, /* This is set dynamically */
      },
      NULL
    },
    {
      "opera_cpu_overclock",
      "CPU超频",
      "对3DO的12.5MHz ARM60 CPU进行超频。\n"
      "很多游戏可以大幅提高帧率（例如极品飞车），但是会增加对系统的性能要求，\n"
      "并且在有些情况下没有效果（或者造成问题）。",
      {
        { "1.0x (12.50Mhz)", NULL },
        { "1.1x (13.75Mhz)", NULL },
        { "1.2x (15.00Mhz)", NULL },
        { "1.5x (18.75Mhz)", NULL },
        { "1.6x (20.00Mhz)", NULL },
        { "1.8x (22.50Mhz)", NULL },
        { "2.0x (25.00Mhz)", NULL },
        { NULL, NULL },
      },
      "1.0x (12.50Mhz)"
    },
    {
      "opera_region",
      "制式",
      "选择分辨率和场频。注意：某些欧版游戏需要欧版ROM。",
      {
        { "ntsc", "NTSC 320x240@60" },
        { "pal1", "PAL1 320x288@50" },
        { "pal2", "PAL2 352x288@50" },
        { NULL, NULL }
      },
      "ntsc"
    },
    {
      "opera_vdlp_pixel_format",
      "VDLP像素格式",
      "选择运行时请求的像素格式，内部16bpp格式会转换成此格式。",
      {
        { "0RGB1555", NULL },
        { "RGB565",   NULL },
        { "XRGB8888", NULL }
      },
      "XRGB8888"
    },
    {
      "opera_vdlp_bypass_clut",
      "VDLP绕过CLUT",
      "强制3DO VDLP绕过游戏的CLUT（色彩查询表）。\n"
      "可能引起不正确的色彩，但是渲染更快。",
      {
        { "disabled", "禁用" },
        { "enabled",  "启用" },
        { NULL, NULL }
      },
      "disabled"
    },
    {
      "opera_high_resolution",
      "高分辨率CEL渲染",
      "启用此选项使CEL以2倍分辨率渲染, 增加3D模型的精确度，对2D游戏无效。\n"
	  "对性能有很大的影响。",
      {
        { "disabled", "禁用" },
        { "enabled",  "启用" },
        { NULL, NULL },
      },
      "disabled"
    },
    {
      "opera_madam_matrix_engine",
      "MADAM矩阵引擎",
      "'MADAM'时3DO的图形加速器，它包含一个定制的数字协处理器，由CPU用来完成矩阵操作，\n"
      "这对应于以‘硬件’方式运行。但是3DO同时有一个内建的ARM‘软件’版本的MADAM矩阵引擎。\n"
      "‘硬件’模式是缺省模式，但是发现某些游戏在强制使用‘软件’模式时运行更快（例如极品飞车）。",
      {
        { "hardware", "硬件" },
        { "software", "软件" },
        { NULL, NULL },
      },
      "hardware"
    },
    {
      "opera_swi_hle",
      "OperaOS SWI高级模拟（HLE）",
      "3DO游戏构建在Portfolio操作系统之上，Opera实现了有些操作系统功能的高级模拟（HLE)， \n"
      "此HLE可以提高特定游戏的性能。",
      {
        { "disabled", "禁用" },
        { "enabled",  "启用" },
        { NULL, NULL }
      },
      "disabled"
    },
#if THREADED_DSP
    {
      "opera_dsp_threaded",
      "多线程DSP",
      "在单独线程中运行DSP（音频处理器）。在多核系统上可以提高性能。！试验性功能！",
      {
        { "disabled", "禁用" },
        { "enabled",  "启用" },
        { NULL, NULL },
      },
      "disabled"
    },
#endif
    {
      "opera_nvram_storage",
      "NVRAM存储",
      "选择NVRAM按游戏存储还是保存为单个文件，在所有游戏间共享。",
      {
        { "per game", "按游戏" },
        { "shared",   "共享" },
        { NULL, NULL },
      },
      "per game"
    },
    {
      "opera_active_devices",
      "活动输入设备",
      "存在一个bug（可能是Opera本身的，但也可能是特定游戏的），\n"
	  "可能使多于一个的模拟控制器造成游戏忽略手柄输入。\n"
      "把'活动输入设备'设为1可以临时解决这个问题。",
      {
        { "0", NULL },
        { "1", NULL },
        { "2", NULL },
        { "3", NULL },
        { "4", NULL },
        { "5", NULL },
        { "6", NULL },
        { "7", NULL },
        { "8", NULL },
        { NULL, NULL },
      },
      "1"
    },
    {
      "opera_hack_timing_1",
      "时序修正1 (Crash 'n Burn)",
      "此项是使游戏'Crash 'n Burn'能正确运行的特殊处理。",
      {
        { "disabled", "禁用" },
        { "enabled",  "启用" },
        { NULL, NULL },
      },
      "disabled"
    },
    {
      "opera_hack_timing_3",
      "时序修正3 (Dinopark Tycoon)",
      "此项是使游戏'Dinopark Tycoon'能正确运行的特殊处理。",
      {
        { "disabled", "禁用" },
        { "enabled",  "启用" },
        { NULL, NULL },
      },
      "disabled"
    },
    {
      "opera_hack_timing_5",
      "时序修正5 (Microcosm)",
      "此项是使游戏'Microcosm'能正确运行的特殊处理。",
      {
        { "disabled", "禁用" },
        { "enabled",  "启用" },
        { NULL, NULL },
      },
      "disabled"
    },
    {
      "opera_hack_timing_6",
      "时序修正6 (Alone in the Dark)",
      "此项是使游戏'Alone in the Dark'能正确运行的特殊处理。",
      {
        { "disabled", "禁用" },
        { "enabled",  "启用" },
        { NULL, NULL },
      },
      "disabled"
    },
    {
      "opera_hack_graphics_step_y",
      "图像Y步进修正 (Samurai Showdown)",
      "此项是使游戏'Samurai Showdown'背景能正确渲染的特殊处理。",
      {
        { "disabled", "禁用" },
        { "enabled",  "启用" },
        { NULL, NULL },
      },
      "disabled"
    },
    {
      "opera_kprint",
      "调试信息输出",
      "输出3DO调试信息到stderr，这只对于3DO自制程序开发者很有用。",
      {
        { "disabled", "禁用" },
        { "enabled",  "启用" },
        { NULL, NULL },
      },
      "disabled"
    },
    { NULL, NULL, NULL, {{0}}, NULL },
  };

/* RETRO_LANGUAGE_ESPERANTO */

/* RETRO_LANGUAGE_POLISH */

/* RETRO_LANGUAGE_VIETNAMESE */

/* RETRO_LANGUAGE_ARABIC */

/* RETRO_LANGUAGE_GREEK */

/* RETRO_LANGUAGE_TURKISH */

/*
********************************
* Language Mapping
********************************
*/

static struct retro_core_option_definition *option_defs_intl[RETRO_LANGUAGE_LAST] = {
  option_defs_us, /* RETRO_LANGUAGE_ENGLISH */
  NULL,           /* RETRO_LANGUAGE_JAPANESE */
  NULL,           /* RETRO_LANGUAGE_FRENCH */
  NULL,           /* RETRO_LANGUAGE_SPANISH */
  NULL,           /* RETRO_LANGUAGE_GERMAN */
  NULL,           /* RETRO_LANGUAGE_ITALIAN */
  NULL,           /* RETRO_LANGUAGE_DUTCH */
  NULL,           /* RETRO_LANGUAGE_PORTUGUESE_BRAZIL */
  NULL,           /* RETRO_LANGUAGE_PORTUGUESE_PORTUGAL */
  NULL,           /* RETRO_LANGUAGE_RUSSIAN */
  NULL,           /* RETRO_LANGUAGE_KOREAN */
  NULL,           /* RETRO_LANGUAGE_CHINESE_TRADITIONAL */
  option_defs_chs,/* RETRO_LANGUAGE_CHINESE_SIMPLIFIED */
  NULL,           /* RETRO_LANGUAGE_ESPERANTO */
  NULL,           /* RETRO_LANGUAGE_POLISH */
  NULL,           /* RETRO_LANGUAGE_VIETNAMESE */
  NULL,           /* RETRO_LANGUAGE_ARABIC */
  NULL,           /* RETRO_LANGUAGE_GREEK */
  NULL,           /* RETRO_LANGUAGE_TURKISH */
};

/*
********************************
* Functions
********************************
*/

static const char bios_disabled_str[] = "disabled";

static
bool
file_exists_in_system_directory(const char *filename)
{
  int ret;
  char fullpath[PATH_MAX_LENGTH];
  const char *system_path = NULL;

  if (!retro_environment_cb)
    return false;

  ret = retro_environment_cb(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY, &system_path);

  if((ret == 0) || !system_path)
    return false;

  fullpath[0] = '\0';
  fill_pathname_join(fullpath, system_path, filename, sizeof(fullpath));

  return path_is_valid(fullpath);
}

static void set_bios_values(struct retro_core_option_value *values)
{
  size_t i                  = 0;
  const opera_bios_t *bios = NULL;

  if (!values)
    return;

  /* Loop through all recognised bios files */
  for(bios = opera_bios_begin(); bios != opera_bios_end(); bios++)
    {
      /* Sanity check */
      if (i >= RETRO_NUM_CORE_OPTION_VALUES_MAX - 1)
        break;

      if (file_exists_in_system_directory(bios->filename))
        {
          values[i].value = bios->name;
          values[i].label = NULL;
          i++;
        }
    }

  /* Handle 'no files found' condition */
  if (i == 0)
    {
      values[i].value = bios_disabled_str;
      values[i].label = NULL;
      i++;
    }

  /* Add proper null termination */
  values[i].value = NULL;
  values[i].label = NULL;
}

static void set_font_values(struct retro_core_option_value *values)
{
  size_t i = 0;
  const opera_bios_t *font = NULL;

  if (!values)
    return;

  /* First value is always 'disabled' */
  values[i].value = bios_disabled_str;
  values[i].label = NULL;
  i++;

  /* Loop through all recognised font files */
  for(font = opera_bios_font_begin(); font != opera_bios_font_end(); font++)
    {
      /* Sanity check */
      if (i >= RETRO_NUM_CORE_OPTION_VALUES_MAX - 1)
        break;

      if (file_exists_in_system_directory(font->filename))
        {
          values[i].value = font->name;
          values[i].label = NULL;
          i++;
        }
    }

  /* Add proper null termination */
  values[i].value = NULL;
  values[i].label = NULL;
}

void
libretro_init_core_options(void)
{
  size_t i = 0;

  /* Loop over options until we find the entries
   * with dynamic values */
  while (true)
    {
      const char *key                        = option_defs_us[i].key;
      struct retro_core_option_value *values = option_defs_us[i].values;

      if (key)
        {
          if (strcmp(key, "opera_bios") == 0)
            set_bios_values(values);
          else if (strcmp(key, "opera_font") == 0)
            set_font_values(values);
          i++;
        }
      else
        break;
    }
}

void
libretro_set_core_options(void)
{
  unsigned version = 0;

  if (!retro_environment_cb)
    return;

  if (retro_environment_cb(RETRO_ENVIRONMENT_GET_CORE_OPTIONS_VERSION, &version) && (version == 1))
    {
      struct retro_core_options_intl core_options_intl;
      unsigned language = 0;

      core_options_intl.us    = option_defs_us;
      core_options_intl.local = NULL;

      if (retro_environment_cb(RETRO_ENVIRONMENT_GET_LANGUAGE, &language) &&
          (language < RETRO_LANGUAGE_LAST) && (language != RETRO_LANGUAGE_ENGLISH))
        core_options_intl.local = option_defs_intl[language];

      retro_environment_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_INTL, &core_options_intl);
    }
  else
    {
      size_t i;
      size_t num_options               = 0;
      struct retro_variable *variables = NULL;
      char **values_buf                = NULL;

      /* Determine number of options */
      while (true)
        {
          if (option_defs_us[num_options].key)
            num_options++;
          else
            break;
        }

      /* Allocate arrays */
      variables  = (struct retro_variable *)calloc(num_options + 1, sizeof(struct retro_variable));
      values_buf = (char **)calloc(num_options, sizeof(char *));

      if (!variables || !values_buf)
        goto error;

      /* Copy parameters from option_defs_us array */
      for (i = 0; i < num_options; i++)
        {
          const char *key                        = option_defs_us[i].key;
          const char *desc                       = option_defs_us[i].desc;
          const char *default_value              = option_defs_us[i].default_value;
          struct retro_core_option_value *values = option_defs_us[i].values;
          size_t buf_len                         = 3;
          size_t default_index                   = 0;

          values_buf[i] = NULL;

          if (desc)
            {
              size_t num_values = 0;

              /* Determine number of values */
              while (true)
                {
                  if (values[num_values].value)
                    {
                      /* Check if this is the default value */
                      if (default_value)
                        if (strcmp(values[num_values].value, default_value) == 0)
                          default_index = num_values;

                      buf_len += strlen(values[num_values].value);
                      num_values++;
                    }
                  else
                    break;
                }

              /* Build values string
               * > Note: Opera is unusual in that we have to
               *   support core options with only one value
               *   (the number of 'opera_bios' and 'opera_font'
               *   options depends upon the number of files
               *   present in the user's system directory...) */
              if (num_values > 0)
                {
                  size_t j;

                  buf_len += num_values - 1;
                  buf_len += strlen(desc);

                  values_buf[i] = (char *)calloc(buf_len, sizeof(char));
                  if (!values_buf[i])
                    goto error;

                  strcpy(values_buf[i], desc);
                  strcat(values_buf[i], "; ");

                  /* Default value goes first */
                  strcat(values_buf[i], values[default_index].value);

                  /* Add remaining values */
                  for (j = 0; j < num_values; j++)
                    {
                      if (j != default_index)
                        {
                          strcat(values_buf[i], "|");
                          strcat(values_buf[i], values[j].value);
                        }
                    }
                }
            }

          variables[i].key   = key;
          variables[i].value = values_buf[i];
        }

      /* Set variables */
      retro_environment_cb(RETRO_ENVIRONMENT_SET_VARIABLES, variables);

    error:

      /* Clean up */
      if (values_buf)
        {
          for (i = 0; i < num_options; i++)
            {
              if (values_buf[i])
                {
                  free(values_buf[i]);
                  values_buf[i] = NULL;
                }
            }

          free(values_buf);
          values_buf = NULL;
        }

      if (variables)
        {
          free(variables);
          variables = NULL;
        }
    }
}
