/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** ServerMediaSubsession.h
** 
** -------------------------------------------------------------------------*/

#ifndef SERVER_MEDIA_SUBSESSION
#define SERVER_MEDIA_SUBSESSION

#include <string>
#include <iomanip>
#include <iostream>

// live555
#include <liveMedia.hh>

// forward declaration
class V4L2DeviceSource;

// ---------------------------------
//   BaseServerMediaSubsession
// ---------------------------------
class BaseServerMediaSubsession
{
	public:
		BaseServerMediaSubsession(StreamReplicator* replicator): m_replicator(replicator) {};
	
	public:
		static FramedSource* createSource(UsageEnvironment& env, FramedSource * videoES, const std::string& format);
		static RTPSink* createSink(UsageEnvironment& env, Groupsock * rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, const std::string& format);
		char const* getAuxLine(V4L2DeviceSource* source,unsigned char rtpPayloadType);
		
	protected:
		StreamReplicator* m_replicator;
};

// -----------------------------------------
//    ServerMediaSubsession for Multicast
// -----------------------------------------
class MulticastServerMediaSubsession : public PassiveServerMediaSubsession , public BaseServerMediaSubsession
{
	public:
		static MulticastServerMediaSubsession* createNew(UsageEnvironment& env
								, struct in_addr destinationAddress
								, Port rtpPortNum, Port rtcpPortNum
								, int ttl
								, StreamReplicator* replicator
								, const std::string& format);
		
	protected:
		MulticastServerMediaSubsession(StreamReplicator* replicator, RTPSink* rtpSink, RTCPInstance* rtcpInstance) 
				: PassiveServerMediaSubsession(*rtpSink, rtcpInstance), BaseServerMediaSubsession(replicator), m_rtpSink(rtpSink) {};			

		virtual char const* sdpLines() ;
		virtual char const* getAuxSDPLine(RTPSink* rtpSink,FramedSource* inputSource);
		
	protected:
		RTPSink* m_rtpSink;
		std::string m_SDPLines;
};

// -----------------------------------------
//    ServerMediaSubsession for Unicast
// -----------------------------------------
class UnicastServerMediaSubsession : public OnDemandServerMediaSubsession , public BaseServerMediaSubsession
{
	public:
		static UnicastServerMediaSubsession* createNew(UsageEnvironment& env, StreamReplicator* replicator, const std::string& format);
		
	protected:
		UnicastServerMediaSubsession(UsageEnvironment& env, StreamReplicator* replicator, const std::string& format) 
				: OnDemandServerMediaSubsession(env, False), BaseServerMediaSubsession(replicator), m_format(format) {};
			
		virtual FramedSource* createNewStreamSource(unsigned clientSessionId, unsigned& estBitrate);
		virtual RTPSink* createNewRTPSink(Groupsock* rtpGroupsock,  unsigned char rtpPayloadTypeIfDynamic, FramedSource* inputSource);		
		virtual char const* getAuxSDPLine(RTPSink* rtpSink,FramedSource* inputSource);
					
	protected:
		const std::string m_format;
};

#endif
