#include "Logo_Precomp.hpp"
#include "Logo_Logger.hpp"

using namespace std;

namespace LogRec
{

	IMPLEMENT_SINGLETON(Logger);

  std::string LogBase::m_basedir;

	const char *BASE_DIR = "/home/beyond_boy/logorec/LogRec/log/";

	char *LOG_FILE		;//= const_cast<char*>((LogBase::getBaseDir()+"log/LOGO_LOG.txt").c_str());
	char *LOG_FILE_RT	;//= const_cast<char*>((LogBase::getBaseDir()+"log/LOGO_RT_LOG.txt").c_str());

	std::string slog;
	std::string slogrt;

  void SET_LOG_BASEDIR(const std::string &basedir)
	{
		slog 		= basedir+"log/LOGO_LOG.txt";
		slogrt 	= basedir+"log/LOGO_RT_LOG.txt";
		
		LOG_FILE		= const_cast<char*>((slog.c_str()));
		LOG_FILE_RT	= const_cast<char*>((slogrt.c_str()));  
	 
	}
	const char* strLogLevel[] = 
	{
		"All",
		"Debug",
		"Info",
		"Warn",
		"Error",
		"Fatal",
		"Off"
	};

	LogBase::LogBase()
	{
	  m_bIsGLog = true;
	}

	void LogBase::setLogOption(eLogOption OPT)
	{
		m_logOption = OPT;
	}

	FILE* LogBase::getLogFileHandle()
	{
		return m_pLogStream;
	}

	eLogOption LogBase::getLogOption() const
	{
		return m_logOption;
	}

	void LogBase::setLogInfo(eLogLevel LOG_LEVEL, char* File, int line)
	{
		m_logLevel	 = LOG_LEVEL;
		m_File		 = File;
		m_line		 = line;
	}

	void LogBase::setMinLogLevel(eLogLevel MIN_LOG_LEVEL)
	{
		if (m_bIsGLog)
		{
			m_pLogStream = fopen(LOG_FILE,"a+");
			//cout << "logfile: " << LOG_FILE << endl;
			if (!m_pLogStream)
			{
				fprintf(stderr,"OPEN LOG FILE %s FAILED!\n",LOG_FILE);
			}
			m_logFile = LOG_FILE_RT;
			m_bIsGLog = false;
		}
		m_minLogLevel = MIN_LOG_LEVEL;
	}

	eLogLevel LogBase::getMinLogLevel() const
	{
		return m_minLogLevel;
	}

	eLogLevel Logger::getLogLevel() const
	{
		return m_logLevel;
	}

	char* Logger::getFileName() const
	{
		return m_File;
	}

	int Logger::getLine() const
	{
		return m_line;
	}

	void LogBase::writeLog(const char* fmt, ...)
	{
		//日志级别控制
		if (m_logLevel >= m_minLogLevel)
		{
			char str[30];
			fprintf(stderr,"\n======================Log Begin===================\n");
			time_t now = time(NULL);
			tm datetime = {0};
			datetime = *(localtime(&now));
			//时间信息
			snprintf(str,30,"%d-%02d-%02d %02d:%02d:%02d",datetime.tm_year+1900,
				datetime.tm_mon,datetime.tm_mday,datetime.tm_hour,datetime.tm_min,datetime.tm_sec);
			//位置信息
			fprintf(stderr,"%s %d %s [%s]\n",m_File,m_line,str,strLogLevel[m_logLevel]);
			//日志主要内容
			va_list argp;
			va_start(argp,fmt);
			vfprintf(stderr,fmt,argp);
			va_end(argp);
			fprintf(stderr,"\n======================Log End======================\n");
		}
	}

	void WRITE_LOG_MESSAGE(const char* fmt, ...)
	{
		//日志级别控制
		FILE* fpw = fopen(LOG_FILE_RT,"a+");
		if (LogRec::Logger::getInstance()->getLogLevel() >= LogRec::Logger::getInstance()->getMinLogLevel())
		{
			char str[30];
			if (LogRec::Logger::getInstance()->getLogOption() != LOG_OPTION_INFO && 
				LogRec::Logger::getInstance()->isGlobalLog() )
			{
				fprintf(LogRec::Logger::getInstance()->getLogFileHandle(),"\n======================Log Begin===================\n");
			}
			else
			{
				if (LogRec::Logger::getInstance()->getLogOption() != LOG_OPTION_INFO)
				{
					fprintf(fpw,"\n======================Log Begin===================\n");
				}
			}

			time_t now = time(NULL);
			tm datetime = {0};

		  datetime = *(localtime(&now));

			switch( LogRec::Logger::getInstance()->getLogOption() )
			{
				case LOG_OPTION_ALL:
					//时间信息
					snprintf(str,30,"%d-%02d-%02d %02d:%02d:%02d",datetime.tm_year+1900,
						datetime.tm_mon+1,datetime.tm_mday,datetime.tm_hour,datetime.tm_min,datetime.tm_sec);
					//位置信息
					if ( LogRec::Logger::getInstance()->isGlobalLog() )
					{
						fprintf(LogRec::Logger::getInstance()->getLogFileHandle(),"%s %d %s [%s]\n",LogRec::Logger::getInstance()->getFileName(),
							LogRec::Logger::getInstance()->getLine()+1,str,strLogLevel[LogRec::Logger::getInstance()->getLogLevel()]);
					}
					else 
					{
						fprintf(fpw,"%s %d %s [%s]\n",LogRec::Logger::getInstance()->getFileName(),
							LogRec::Logger::getInstance()->getLine()+1,str,strLogLevel[LogRec::Logger::getInstance()->getLogLevel()]);
					}
					//日志主要内容
					break;
				case LOG_OPTION_FILE:
					//位置信息
					if ( LogRec::Logger::getInstance()->isGlobalLog() )
					{
						fprintf(LogRec::Logger::getInstance()->getLogFileHandle(),"%s %d [%s]\n",LogRec::Logger::getInstance()->getFileName(),
							LogRec::Logger::getInstance()->getLine()+1,strLogLevel[LogRec::Logger::getInstance()->getLogLevel()]);
					}
					else
					{
						fprintf(fpw,"%s %d [%s]\n",LogRec::Logger::getInstance()->getFileName(),
							LogRec::Logger::getInstance()->getLine()+1,strLogLevel[LogRec::Logger::getInstance()->getLogLevel()]);
					}
					break;

				case LOG_OPTION_TIME:
					//时间信息
					snprintf(str,30,"%d-%02d-%02d %02d:%02d:%02d",datetime.tm_year+1900,
						datetime.tm_mon,datetime.tm_mday+1,datetime.tm_hour,datetime.tm_min,datetime.tm_sec);
					if ( LogRec::Logger::getInstance()->isGlobalLog() )
					{					
						fprintf(LogRec::Logger::getInstance()->getLogFileHandle(),"%s [%s]\n",str,strLogLevel[LogRec::Logger::getInstance()->getLogLevel()]);
					}
					else
					{
						fprintf(fpw,"%s [%s]\n",str,strLogLevel[LogRec::Logger::getInstance()->getLogLevel()]);
					}
					break;

				case LOG_OPTION_INFO:
					if ( LogRec::Logger::getInstance()->isGlobalLog() )
					{
						fprintf(LogRec::Logger::getInstance()->getLogFileHandle(),"\n[%s] ",strLogLevel[LogRec::Logger::getInstance()->getLogLevel()]);
					}
					else
					{
						fprintf(fpw,"\n[%s] ",strLogLevel[LogRec::Logger::getInstance()->getLogLevel()]);
					}
					break;

				default:
					break;
			}

			va_list argp;
			va_start(argp,fmt);
			vfprintf(LogRec::Logger::getInstance()->getLogFileHandle(),fmt,argp);

			vfprintf(fpw,fmt,argp);
			va_end(argp);
			if ( LogRec::Logger::getInstance()->getLogOption() != LOG_OPTION_INFO 
				&& LogRec::Logger::getInstance()->isGlobalLog() )
			{
				fprintf(LogRec::Logger::getInstance()->getLogFileHandle(),"\n======================Log End=====================\n");
			}
			else
			{
				if (LogRec::Logger::getInstance()->getLogOption() != LOG_OPTION_INFO)
				{
					fprintf(fpw,"\n======================Log End=====================\n");
				}
			}
			//fputs("dfwefwefg end",LogRec::Logger::getInstance()->getLogFileHandle());
			fclose(fpw);
		}
	}

	void PRINT_LOG_MESSAGE(const char* fmt, ...)
	{
		//日志级别控制
		if (LogRec::Logger::getInstance()->getLogLevel() >= LogRec::Logger::getInstance()->getMinLogLevel())
		{
			char str[30];
			if (LogRec::Logger::getInstance()->getLogOption() != LOG_OPTION_INFO)
			{
				fprintf(stderr,"\n======================Log Begin===================\n");
			}
			time_t now = time(NULL);
			tm datetime = {0};
			datetime = *(localtime(&now));

			switch( LogRec::Logger::getInstance()->getLogOption() )
			{
				case LOG_OPTION_ALL:
					//时间信息
					snprintf(str,30,"%d-%02d-%02d %02d:%02d:%02d",datetime.tm_year+1900,
						datetime.tm_mon+1,datetime.tm_mday,datetime.tm_hour,datetime.tm_min,datetime.tm_sec);
					//位置信息
					fprintf(stderr,"%s %d %s [%s]\n",LogRec::Logger::getInstance()->getFileName(),
						LogRec::Logger::getInstance()->getLine()+1,str,strLogLevel[LogRec::Logger::getInstance()->getLogLevel()]);
					//日志主要内容
					break;
				case LOG_OPTION_FILE:
					//位置信息
					fprintf(stderr,"%s %d [%s]\n",LogRec::Logger::getInstance()->getFileName(),
						LogRec::Logger::getInstance()->getLine()+1,strLogLevel[LogRec::Logger::getInstance()->getLogLevel()]);
					break;

				case LOG_OPTION_TIME:
					//时间信息
					snprintf(str,30,"%d-%02d-%02d %02d:%02d:%02d",datetime.tm_year+1900,
						datetime.tm_mon,datetime.tm_mday+1,datetime.tm_hour,datetime.tm_min,datetime.tm_sec);
	
					fprintf(stderr,"%s [%s]\n",str,strLogLevel[LogRec::Logger::getInstance()->getLogLevel()]);
					break;

				case LOG_OPTION_INFO:
					fprintf(stderr,"\n[%s] ",strLogLevel[LogRec::Logger::getInstance()->getLogLevel()]);
					break;

				default:
					break;
			}

			va_list argp;
			va_start(argp,fmt);
			vfprintf(stderr,fmt,argp);
			va_end(argp);
			if (LogRec::Logger::getInstance()->getLogOption() != LOG_OPTION_INFO)
			{
				fprintf(stderr,"\n======================Log End=====================\n");
			}
		}
	}
	
}
