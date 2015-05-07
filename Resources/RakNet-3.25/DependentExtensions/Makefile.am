lib_LTLIBRARIES =
pkginclude_HEADERS = 

if ENABLE_AUTOPATCHER
lib_LTLIBRARIES 		+= libraknetautopatcher.la
libraknetautopatcher_la_LDFLAGS		= -version-info 0:0:0 $(PTHREAD_LIBS) $(PTHREAD_CFLAGS) $(POSTGRESQL_LDFLAGS)
libraknetautopatcher_la_CPPFLAGS		= $(PTHREAD_CFLAGS) $(POSTGRESQL_CFLAGS) -I$(top_srcdir)/Source -I$(srcdir)/PostgreSQLInterface
libraknetautopatcher_la_SOURCES =	CreatePatch.cpp \
					ApplyPatch.cpp \
					MemoryCompressor.cpp \
					AutopatcherClient.cpp \
					AutopatcherServer.cpp  \
					PostgreSQLInterface/PostgreSQLInterface.cpp \
					AutopatcherPostgreRepository/AutopatcherPostgreRepository.cpp

pkginclude_HEADERS +=	CreatePatch.h \
			ApplyPatch.h \
			MemoryCompressor.h \
			AutopatcherClient.h \
			AutopatcherServer.h \
			PostgreSQLInterface/PostgreSQLInterface.h \
			AutopatcherPostgreRepository/AutopatcherPostgreRepository.h
endif

if ENABLE_RAKVOICE
lib_LTLIBRARIES 		+= libraknetrakvoice.la
libraknetrakvoice_la_LDFLAGS		= -version-info 0:0:0 $(PTHREAD_LIBS) $(PTHREAD_CFLAGS) $(SPEEX_LIBS)
libraknetrakvoice_la_CPPFLAGS		= $(PTHREAD_CFLAGS) $(SPEEX_CFLAGS) -I$(top_srcdir)/Source
libraknetrakvoice_la_SOURCES =	RakVoice.cpp 

pkginclude_HEADERS +=	RakVoice.h 
endif

EXTRA_DIST = PatchTest bzip2-1.0.3 portaudio_v18_1 speex-1.1.12 Lobby openssl-0.9.8g Ogre3DInterpDemo AutopatcherPostgreRepository/AutopatcherPostgreRepository.vcproj AutopatcherPostgreRepository/AutopatcherPostgreRepository.vcproj.user AutopatcherPostgreRepository/AutopatcherPostgreRepository_vc8.vcproj
