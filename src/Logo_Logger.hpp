#ifndef	__LOGO_LOGGER_H__
#define	__LOGO_LOGGER_H__	

namespace LogRec
{
	/***
	  *
	  *
	  *单例模式的两种复用方式
	  *
	  ***/
	template<typename T>
	class Singleton 
	{
	public:
		static T* getInstance()
		{
			if (m_instance == NULL)
			{
				m_instance = new T;
			}
			return m_instance;
		}

		static void unGetInstance()
		{
			if (m_instance != NULL)
			{
				delete m_instance;
				m_instance = NULL;
			}
		}

	protected:
		Singleton(){}
		virtual ~Singleton(){}
	private:
		static T* m_instance;
	};


	template <typename T> 
	T* Singleton<T>::m_instance = NULL;

	//宏定义方式
	#define	DECLARE_SINGLETON(Singleton)	public:\
		static Singleton* getInstance(){ \
			if (m_instance == NULL){\
				m_instance = new Singleton;\
			}\
			return m_instance;\
		}\
		static void unInstance(){\
			if (m_instance != NULL){\
				delete m_instance;\
				m_instance = NULL;\
			}\
		}\
	private:\
		Singleton(){}\
		virtual ~Singleton(){\
			if (m_instance != NULL){\
				delete m_instance;\
				m_instance = NULL;\
			}\
		}\
	private:\
		static Singleton* m_instance;\

	#define IMPLEMENT_SINGLETON(Singleton)	Singleton* Singleton::m_instance = NULL;


	/**
	  *
	  *
	  *简单日志类的基本实现
	  *
	  */

	//define the log level
	enum eLogLevel
	{
		LEVEL_ALL	=	0,
		LEVEL_DEBUG,
		LEVEL_INFO,
		LEVEL_WARN,
		LEVEL_ERROR,
		LEVEL_FATAL,
		LEVEL_OFF
	};

	enum eLogOption
	{
		LOG_OPTION_ALL	=	0,
		LOG_OPTION_FILE,
		LOG_OPTION_TIME,
		LOG_OPTION_INFO
	};

	/*#define LOG_FILE	"D:/LOGO/Log/LOGO_LOG.txt"
	#define LOG_FILE_RT	"D:/LOGO/Log/LOGO_LOG_RT.txt"*/



	class LogBase
	{
	public:
		void setLogInfo(eLogLevel LOG_LEVEL, char* File, int line);
		void setMinLogLevel(eLogLevel MIN_LOG_LEVEL);
		eLogLevel getMinLogLevel() const;
		virtual void writeLog(const char* fmt, ...);
		
		void setLogOption(eLogOption OPT);
		eLogOption getLogOption() const;
	
		LogBase();

		bool isGlobalLog()
		{
			return m_bIsGLog;
		}
		void setGlobalLog()
		{
			m_bIsGLog = true;
		}
		virtual ~LogBase()
		{
			fclose(m_pLogStream);
		}

		FILE *getLogFileHandle();

		void Loginfo()
		{
			std::cout << "hello world!" << std::endl;
		}
 		
		static bool setBaseDir(std::string basedir)
		{
		  m_basedir = basedir;
		  return true;
		}
	
	  static std::string& getBaseDir()
		{
		  return m_basedir;
		}
	private:
		eLogLevel m_minLogLevel;

	protected:	
	  static std::string m_basedir;
		bool m_bIsGLog;
		FILE *m_pLogStream;
		char *m_logFile;
		char* m_File;
		int m_line;
		std::string m_sLogFile;
		std::string m_sLogFilePath;
		eLogLevel m_logLevel;
		eLogOption m_logOption;
	};

	class Logger : public LogBase
	{
		DECLARE_SINGLETON(Logger);
	public:
		eLogLevel getLogLevel() const;
		char* getFileName() const;
		int getLine() const;
	};


	#define LOG_SET_MIN_LEVEL(minLevel)	Logger::getInstance()->setMinLogLevel(minLevel);
	#define LOG_SET_OPTION(option)		Logger::getInstance()->setLogOption(option);
	#define LOG_SET_LOGFILE_PATH(path)		Logger::getInstance()->setLogFilePath(path);
	#define LOG_SET_LOGFILE(file)		Logger::getInstance()->setLogFile(file);


	#define LOG_LEVEL_ALL		Logger::getInstance()->setLogInfo(LEVEL_ALL,__FILE__,__LINE__);
	#define LOG_LEVEL_INFO		Logger::getInstance()->setLogInfo(LEVEL_INFO,__FILE__,__LINE__);
	#define LOG_LEVEL_WARN		Logger::getInstance()->setLogInfo(LEVEL_WARN,__FILE__,__LINE__);
	#define LOG_LEVEL_ERROR		Logger::getInstance()->setLogInfo(LEVEL_ERROR,__FILE__,__LINE__);
	#define LOG_LEVEL_FATAL		Logger::getInstance()->setLogInfo(LEVEL_FATAL,__FILE__,__LINE__);
	#define LOG_LEVEL_OFF		Logger::getInstance()->setLogInfo(LEVEL_OFF,__FILE__,__LINE__);
	//该句相当于定义和实例化一个静态变量，不能放在头文件中，否则一旦多个文件包含该头文件，就会报该静态变量已经在多个目标文件中定义的错误
	//IMPLEMENT_SINGLETON(Logger);
	void SET_LOG_BASEDIR(const std::string& basedir);
	void WRITE_LOG_MESSAGE(const char* fmt, ...);
	void PRINT_LOG_MESSAGE(const char* fmt, ...);
}

#endif	LOG_H
