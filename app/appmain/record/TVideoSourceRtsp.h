#ifndef TVIDEOSOURCERTSP_H
#define TVIDEOSOURCERTSP_H


#include<iostream>
#include<string>

#include "TVideoSource.h"
#include "TLive555EnvThread.h"

using namespace std;

class StreamClientState {
public:
  StreamClientState();
  virtual ~StreamClientState();

public:
  MediaSubsessionIterator* iter;
  MediaSession* session;
  MediaSubsession* subsession;
  TaskToken streamTimerTask;
  double duration;
};

class ourRTSPClient: public RTSPClient {
public:
  static ourRTSPClient* createNew(UsageEnvironment& env, char const* rtspURL,
				  int verbosityLevel = 0,
				  char const* applicationName = NULL,
				  portNumBits tunnelOverHTTPPortNum = 0);

protected:
  ourRTSPClient(UsageEnvironment& env, char const* rtspURL,
		int verbosityLevel, char const* applicationName, portNumBits tunnelOverHTTPPortNum);
    // called only by createNew();
  virtual ~ourRTSPClient();

public:
  StreamClientState scs;
};


class DummySink: public MediaSink {
public:
  static DummySink* createNew(UsageEnvironment& env,
			      MediaSubsession& subsession, // identifies the kind of data that's being received
			      char const* streamId = NULL); // identifies the stream itself (optional)

private:
  DummySink(UsageEnvironment& env, MediaSubsession& subsession, char const* streamId);
    // called only by "createNew()"
  virtual ~DummySink();

  static void afterGettingFrame(void* clientData, unsigned frameSize,
                                unsigned numTruncatedBytes,
				struct timeval presentationTime,
                                unsigned durationInMicroseconds);
  void afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes,
			 struct timeval presentationTime, unsigned durationInMicroseconds);

private:
  // redefined virtual functions:
  virtual Boolean continuePlaying();

private:
  u_int8_t* fReceiveBuffer;
  MediaSubsession& fSubsession;
  char* fStreamId;
};

class TVideoSourceRtsp:public TVideoSource
{
public:
    TVideoSourceRtsp();
    ~TVideoSourceRtsp();
    bool openRtsp(const char* url, const char* username, const char* pwd);
private:
    static void shutdownStream(RTSPClient* rtspClient, int exitCode = 0);
    static void streamTimerHandler(void* clientData);
    static void setupNextSubsession(RTSPClient* rtspClient);
    static void subsessionAfterPlaying(void* clientData);
    static void subsessionByeHandler(void* clientData, char const* reason);
    static void continueAfterSETUP(RTSPClient* rtspClient, int resultCode, char* resultString);
    static void continueAfterPLAY(RTSPClient* rtspClient, int resultCode, char* resultString);
    static void continueAfterDESCRIBE(RTSPClient* rtspClient, int resultCode, char* resultString);

private:
    string          m_rtspUrl;
    string          m_username;
    string          m_pwd;
    UsageEnvironment*  m_env;
    ourRTSPClient*     m_ourRTSPClient;
};

#endif