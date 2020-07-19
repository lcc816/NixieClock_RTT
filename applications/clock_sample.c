#include <rtthread.h>
#include <rtdevice.h>
#include <stdlib.h> // for atoi
#include "i2c_adapter.h"
#include "ds3231.h"
#include "optparse.h"

typedef uint8_t arg_buff_t[8];

static struct optparse_long long_opts[] = 
{
    {"show", 's', OPTPARSE_NONE},
    {"time", 't', OPTPARSE_OPTIONAL},
    {"date", 'd', OPTPARSE_OPTIONAL},
    {"clock", 'c', OPTPARSE_OPTIONAL},
    {"alarm", 'a', OPTPARSE_OPTIONAL},
    {"help", 'h', OPTPARSE_NONE},
    { NULL,  0,  OPTPARSE_NONE}
};

static void ds3231_show_all(void)
{
    DS3231_Time time;
    rt_bool_t a2ie, a1ie;
    rt_bool_t a2f, a1f;

    DS3231_GetTime(&time);
    rt_kprintf("date:   %02u-%02u-%02u\n", time.year, time.month, time.date);
    rt_kprintf("clock:  %02u:%02u:%02u\n", time.hour, time.minute, time.second);
    a2ie = DS3231_CheckAlarmITEnabled(2);
    a1ie = DS3231_CheckAlarmITEnabled(1);
    rt_kprintf("A2IE: %d, A1IE: %d\n", a2ie, a1ie);
    a2f = DS3231_CheckIfAlarm(2);
    a1f = DS3231_CheckIfAlarm(1);
    rt_kprintf("A2F:  %d, A1F:  %d\n", a2f, a1f);
}

/*******************************************************************************
* @brief    Parsing input argument.
* @param    arg - pointer to the input argument.
* @param    buff - buffer to save the parsed argument.
* @retval   bytes of parsed argument, or a negative value if error occurred.
*******************************************************************************/
static int arg_parse(char *arg, arg_buff_t buff)
{
    int i, j;
    char num[3];

    if (!arg)
    {
        return -1;
    }

    if (!buff)
    {
        return -1;
    }

    for (j = 0; j < sizeof(arg_buff_t); j++)
    {
        rt_memset(num, 0, sizeof num);
        i = 0;
        while ((*arg != ',') && (*arg != '\0') && (i < 3))
        {
            num[i++] = *arg++;
        }
        if (i == 3)
        {
            rt_kprintf("parsing error: digit exceeds 2\n");
            return -1;
        }
        buff[j] = atoi(num);
        if (*arg == '\0')
            break;
        arg++;
    }

    return j;
}

/*
 * 返回值:   0: 成功
 *          -1: 参数为空
 *          -2: 解析错误
 *          -3: 其他
 */
static int ds3231_set_time(char *arg)
{
    arg_buff_t sztime;
    DS3231_Time time;
    int len;

    if (!arg)
    {
        DS3231_GetTime(&time);
        rt_kprintf("date:   %02u-%02u-%02u\n", time.year, time.month, time.date);
        rt_kprintf("clock:  %02u:%02u:%02u\n", time.hour, time.minute, time.second);
        return 0;
    }

    len = arg_parse(arg, sztime);
    if (len < 0)
    {
        return -2;
    }
    if (len < 6)
    {
        rt_kprintf("error: numbers not enough\n");
        return -2;
    }

    time.year = sztime[0];
    time.month = sztime[1];
    time.date = sztime[2];
    time.hour = sztime[3];
    time.minute = sztime[4];
    time.second = sztime[5];

    rt_kprintf("set time to: %02d-%02d-%02d, %02d:%02d:%02d\n", 
                time.year, time.month, time.date, time.hour, time.minute, time.second);
    DS3231_SetTime(&time);

    return 0;
}

static int ds3231_set_date(char *arg)
{
    arg_buff_t szbuff;
    int len;
    DS3231_Date date;

    if (!arg)
    {
        DS3231_GetDate(&date);
        rt_kprintf("date:   %02u-%02u-%02u\n", date.year, date.month, date.date);
        return 0;
    }

    len = arg_parse(arg, szbuff);
    if (len < 0)
    {
        return -2;
    }
    if (len < 3)
    {
        rt_kprintf("error: numbers not enough\n");
        return -2;
    }

    date.year = szbuff[0];
    date.month = szbuff[1];
    date.date = szbuff[2];

    rt_kprintf("set date to: %02d-%02d-%02d\n", date.year, date.month, date.date);
    DS3231_SetDate(&date);

    return 0;
}

static int ds3231_set_clock(char *arg)
{
    arg_buff_t szbuff;
    int len;
    DS3231_Clock clock;

    if (!arg)
    {
        DS3231_GetClock(&clock);
        rt_kprintf("clock:  %02u:%02u:%02u\n", clock.hour, clock.minute, clock.second);
        return 0;
    }

    len = arg_parse(arg, szbuff);
    if (len < 0)
    {
        return -2;
    }
    if (len < 3)
    {
        rt_kprintf("parsing error: numbers not enough\n");
        return -2;
    }

    clock.hour = szbuff[0];
    clock.minute = szbuff[1];
    clock.second = szbuff[2];

    rt_kprintf("set clock to: %02d:%02d:%02d\n", clock.hour, clock.minute, clock.second);
    DS3231_SetClock(&clock);

    return 0;
}

/* 支持设置时, 分, 秒 */
static int ds3231_set_alarm(char *arg)
{
    arg_buff_t szbuff;
    int len;
    DS3231_Time alarm;

    if (!arg)
    {
        rt_kprintf("error: argument is null\n");
        return -1;
    }

    len = arg_parse(arg, szbuff);
    if (len < 0)
    {
        return -2;
    }
    if (len < 3)
    {
        rt_kprintf("parsing error: numbers not enough\n");
        return -2;
    }

    alarm.hour = szbuff[0];
    alarm.minute = szbuff[1];
    alarm.second = szbuff[2];

    rt_kprintf("set alarm to: %02d:%02d:%02d\n", alarm.hour, alarm.minute, alarm.second);
    DS3231_SetAlarm1(DS3231_A1_HourMinuteSecond, &alarm);
    DS3231_EnableAlarmIT(1);

    return 0;
}

static void ds3231_show_help(void)
{
    rt_kprintf(
        "all arguments:\n"
        "-s, --show     show all information\n"
        "-t, --time     setting year, month, date, hour, minute, sencond\n"
        "-d, --date     setting year, month, date\n"
        "-c, --clock    setting hour, minute, sencond\n"
        "-a, --alarm    setting a clock like hour, minute, sencond\n"
        "-h, --help     show this help\n"
        "\n"
    );
}

static int ds3231_clock_example(int argc, char **argv)
{
    int ch; 
    struct optparse options;

    if (RT_TRUE != i2c_initialized)
    {
        if (RT_EOK != I2c_Init("i2c1"))
        {
            return -EXIT_FAILURE;
        }
    }

    optparse_init(&options, argv); 
    while((ch = optparse_long(&options, long_opts, NULL)) != -1)
    {
        switch (ch)
        {
            case 's':
                ds3231_show_all();
                break;
            case 't':
                ds3231_set_time(options.optarg);
                break;
            case 'd':
                ds3231_set_date(options.optarg);
                break;
            case 'c':
                ds3231_set_clock(options.optarg);
                break;
            case 'a':
                ds3231_set_alarm(options.optarg);
                break;
            case 'h':
                ds3231_show_help();
                break;
            case '?':
                rt_kprintf("error: invalid argument\n");
                return -EXIT_FAILURE;
        }
    }
    rt_kprintf("\n");

    return 0; 
}

MSH_CMD_EXPORT_ALIAS(ds3231_clock_example, rtc, a clock application based ds3231);
