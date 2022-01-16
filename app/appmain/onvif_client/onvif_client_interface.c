/*
 * onvif_client_interface.cpp
 *
 *  Created on: 2021年12月9日
 *      Author: jhting
 */


#include "onvif_client_interface.h"


#include "discoveryStub.h"
#include "discoveryH.h"

#include "stdsoap2.h"
#include "wsaapi.h"
#include "wsddapi.h"

#define DS_TYPES "hello"


/* This defines the global XML namespaces[] table to #include and compile */
SOAP_NMAC struct Namespace namespaces[] = {
        { "SOAP-ENV", "http://www.w3.org/2003/05/soap-envelope", "http://schemas.xmlsoap.org/soap/envelope/", NULL },
        { "SOAP-ENC", "http://www.w3.org/2003/05/soap-encoding", "http://schemas.xmlsoap.org/soap/encoding/", NULL },
        { "xsi", "http://www.w3.org/2001/XMLSchema-instance", "http://www.w3.org/*/XMLSchema-instance", NULL },
        { "xsd", "http://www.w3.org/2001/XMLSchema", "http://www.w3.org/*/XMLSchema", NULL },
        { "wsa", "http://schemas.xmlsoap.org/ws/2004/08/addressing", "http://www.w3.org/2005/08/addressing", NULL },
        { "wsdd", "http://schemas.xmlsoap.org/ws/2005/04/discovery", NULL, NULL },
        { "tdn", "http://www.onvif.org/ver10/network/wsdl", NULL, NULL },
        { NULL, NULL, NULL, NULL}
    };

int onvif_client_init()
{
	return 0;
}

void onvif_clinet_exit()
{
	return;
}

void wsdd_event_ProbeMatches(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int
MessageNumber, const char *MessageID, const char *RelatesTo, struct wsdd__ProbeMatchesType *matches)
{
	return;
}

void wsdd_event_ResolveMatches(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int
MessageNumber, const char *MessageID, const char *RelatesTo, struct wsdd__ResolveMatchType *match)
{
	return;
}

void wsdd_event_Hello(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber,
const char *MessageID, const char *RelatesTo, const char *EndpointReference, const char *Types, const char *Scopes, const
char *MatchBy, const char *XAddrs, unsigned int MetadataVersion)
{
	return;
}

void wsdd_event_Bye(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const
char *MessageID, const char *RelatesTo, const char *EndpointReference, const char *Types, const char *Scopes, const char
*MatchBy, const char *XAddrs, unsigned int *MetadataVersion)
{
	return;
}

soap_wsdd_mode wsdd_event_Probe(struct soap *soap, const char *MessageID, const char *ReplyTo, const char *Types, const
char *Scopes, const char *MatchBy, struct wsdd__ProbeMatchesType *matches)
{
	printf("MessageID:%s",MessageID);
	return SOAP_WSDD_MANAGED;
}

soap_wsdd_mode wsdd_event_Resolve(struct soap *soap, const char *MessageID, const char *ReplyTo, const char
*EndpointReference, struct wsdd__ResolveMatchType *match)
{
	return SOAP_WSDD_MANAGED;
}

int recv_callback(struct soap *gsoap,char *xml,int len)
{
		printf("============================recv xml===============================:\n%s\n",xml);
		return 0;
}

int onvif_search_start()
{
	struct soap soap;
	struct SOAP_ENV__Header header;
	struct ip_mreq mcast;


	soap_set_namespaces(&soap, namespaces);
	soap_init(&soap);

	printf("[%s][%d][%s][%s] ServerSoap.version = %d \n", __FILE__, __LINE__, __TIME__, __func__, soap.version);

	soap.connect_flags = SO_BROADCAST;
	soap.send_timeout = 1; // 1s timeout
	soap.recv_timeout = 1;

	const char* msg_uuid = soap_wsa_rand_uuid(&soap);
	//soap.header = &header;
#if 0
	mcast.imr_multiaddr.s_addr = inet_addr("239.255.255.250");
	mcast.imr_interface.s_addr = htonl(INADDR_ANY);

	if(setsockopt(soap.master, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mcast, sizeof(mcast)) < 0)
	{
		printf("setsockopt error! error code = %d,err string = %s\n", errno, strerror(errno));
		return 0;
	}

	#define MACH_ADDR_LENGTH 6
	#define INFO_LENGTH 512
	#define LARGE_INFO_LENGTH 1024
	#define SMALL_INFO_LENGTH 512
	unsigned char macaddr[6] = { 0 };
	char _IPAddr[INFO_LENGTH] = { 0 };
	char _HwId[1024] = { 0 };

	macaddr[0] = 0x01; macaddr[1] = 0x0c; macaddr[2] = 0x29; macaddr[3] = 0xbd; macaddr[4] = 0xa6; macaddr[5] = 0x87;
	sprintf(_HwId, "urn:uuid:2419d68a-2dd2-21b2-a205-%02X%02X%02X%02X%02X%02X", macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5]);

	sprintf(_IPAddr, "http://%s/onvif/device_service", "192.168.112.20");
	printf("[%d] _IPAddr ==== %s\n", __LINE__, _IPAddr);

	soap.header->wsa__MessageID = (char *)soap_malloc(&soap, sizeof(char)* INFO_LENGTH);
	strcpy(soap.header->wsa__MessageID, _HwId + 4);


	soap.header->wsa__To = "http://schemas.xmlsoap.org/ws/2004/08/addressing/role/anonymous";
	soap.header->wsa__Action = "http://schemas.xmlsoap.org/ws/2005/04/discovery/ProbeMatches";
	soap.header->wsa__RelatesTo = (struct wsa__Relationship*)soap_malloc(&soap, sizeof(struct wsa__Relationship));
	soap.header->wsa__RelatesTo->__item = soap.header->wsa__MessageID;
	printf("__item: %p, wsa__MessageID: %p: %s\n", soap.header->wsa__RelatesTo->__item, soap.header->wsa__MessageID, soap.header->wsa__MessageID);
	soap.header->wsa__RelatesTo->RelationshipType = NULL;
	soap.header->wsa__RelatesTo->__anyAttribute = NULL;
#endif
	soap_wsdd_Probe(&soap,SOAP_WSDD_MANAGED,SOAP_WSDD_TO_TS,"soap.udp://239.255.255.250:3702",
			msg_uuid,NULL,DS_TYPES,"",NULL);
#if 0
	if (SOAP_OK == soap_send___wsdd__Probe(&soap, "http://", NULL, NULL))
	{
		printf("send soap_send___wsdd__Probe success !\n");
	}else{
		printf("send soap_send___wsdd__Probe failure! !\n");
	}
#endif


	soap.frecv = recv_callback;
	struct __wsdd__Probe __wsdd__probe_recv;
	for(int i = 0; i < 10; i++)
	{
	    soap_recv___wsdd__Probe(&soap,&__wsdd__probe_recv);
	    sleep(1);
	}
	soap_destroy(&soap);
	soap_end(&soap);
	soap_done(&soap);

	return 0;
}

void onvif_search_stop()
{
	return;
}

int main(int argc, char** argv)
{
	onvif_search_start();
	do{
		sleep(1);
	}while(1);

	return 0;
}
