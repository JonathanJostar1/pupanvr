/* imageingServer.cpp
   Generated by gSOAP 2.8.117 for out/onvif_imaging.h

gSOAP XML Web services tools
Copyright (C) 2000-2021, Robert van Engelen, Genivia Inc. All Rights Reserved.
The soapcpp2 tool and its generated software are released under the GPL.
This program is released under the GPL with the additional exemption that
compiling, linking, and/or using OpenSSL is allowed.
--------------------------------------------------------------------------------
A commercial use license is available from Genivia Inc., contact@genivia.com
--------------------------------------------------------------------------------
*/

#if defined(__BORLANDC__)
#pragma option push -w-8060
#pragma option push -w-8004
#endif
#include "imageingH.h"

SOAP_SOURCE_STAMP("@(#) imageingServer.cpp ver 2.8.117 2021-12-08 15:16:43 GMT")
extern "C" SOAP_FMAC5 int SOAP_FMAC6 soap_serve(struct soap *soap)
{
#ifndef WITH_FASTCGI
	soap->keep_alive = soap->max_keep_alive + 1;
#endif
	do
	{
#ifndef WITH_FASTCGI
		if (soap->keep_alive > 0 && soap->max_keep_alive > 0)
			soap->keep_alive--;
#endif
		if (soap_begin_serve(soap))
		{	if (soap->error >= SOAP_STOP)
				continue;
			return soap->error;
		}
		if ((soap_serve_request(soap) || (soap->fserveloop && soap->fserveloop(soap))) && soap->error && soap->error < SOAP_STOP)
		{
#ifdef WITH_FASTCGI
			soap_send_fault(soap);
#else
			return soap_send_fault(soap);
#endif
		}
#ifdef WITH_FASTCGI
		soap_destroy(soap);
		soap_end(soap);
	} while (1);
#else
	} while (soap->keep_alive);
#endif
	return SOAP_OK;
}

#ifndef WITH_NOSERVEREQUEST
extern "C" SOAP_FMAC5 int SOAP_FMAC6 soap_serve_request(struct soap *soap)
{
	(void)soap_peek_element(soap);
	if (!soap_match_tag(soap, soap->tag, "SOAP-ENV:Fault"))
		return soap_serve_SOAP_ENV__Fault(soap);
	if (!soap_match_tag(soap, soap->tag, "timg:GetServiceCapabilities"))
		return soap_serve___timg__GetServiceCapabilities(soap);
	if (!soap_match_tag(soap, soap->tag, "timg:GetImagingSettings"))
		return soap_serve___timg__GetImagingSettings(soap);
	if (!soap_match_tag(soap, soap->tag, "timg:SetImagingSettings"))
		return soap_serve___timg__SetImagingSettings(soap);
	if (!soap_match_tag(soap, soap->tag, "timg:GetOptions"))
		return soap_serve___timg__GetOptions(soap);
	if (!soap_match_tag(soap, soap->tag, "timg:Move"))
		return soap_serve___timg__Move(soap);
	if (!soap_match_tag(soap, soap->tag, "timg:Stop"))
		return soap_serve___timg__Stop(soap);
	if (!soap_match_tag(soap, soap->tag, "timg:GetStatus"))
		return soap_serve___timg__GetStatus(soap);
	if (!soap_match_tag(soap, soap->tag, "timg:GetMoveOptions"))
		return soap_serve___timg__GetMoveOptions(soap);
	if (!soap_match_tag(soap, soap->tag, "timg:GetPresets"))
		return soap_serve___timg__GetPresets(soap);
	if (!soap_match_tag(soap, soap->tag, "timg:GetCurrentPreset"))
		return soap_serve___timg__GetCurrentPreset(soap);
	if (!soap_match_tag(soap, soap->tag, "timg:SetCurrentPreset"))
		return soap_serve___timg__SetCurrentPreset(soap);
	return soap->error = SOAP_NO_METHOD;
}
#endif

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_SOAP_ENV__Fault(struct soap *soap)
{	struct SOAP_ENV__Fault soap_tmp_SOAP_ENV__Fault;
	soap_default_SOAP_ENV__Fault(soap, &soap_tmp_SOAP_ENV__Fault);
	if (!soap_get_SOAP_ENV__Fault(soap, &soap_tmp_SOAP_ENV__Fault, "SOAP-ENV:Fault", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = SOAP_ENV__Fault(soap, soap_tmp_SOAP_ENV__Fault.faultcode, soap_tmp_SOAP_ENV__Fault.faultstring, soap_tmp_SOAP_ENV__Fault.faultactor, soap_tmp_SOAP_ENV__Fault.detail, soap_tmp_SOAP_ENV__Fault.SOAP_ENV__Code, soap_tmp_SOAP_ENV__Fault.SOAP_ENV__Reason, soap_tmp_SOAP_ENV__Fault.SOAP_ENV__Node, soap_tmp_SOAP_ENV__Fault.SOAP_ENV__Role, soap_tmp_SOAP_ENV__Fault.SOAP_ENV__Detail);
	if (soap->error)
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve___timg__GetServiceCapabilities(struct soap *soap)
{	struct __timg__GetServiceCapabilities soap_tmp___timg__GetServiceCapabilities;
	_timg__GetServiceCapabilitiesResponse timg__GetServiceCapabilitiesResponse;
	timg__GetServiceCapabilitiesResponse.soap_default(soap);
	soap_default___timg__GetServiceCapabilities(soap, &soap_tmp___timg__GetServiceCapabilities);
	if (!soap_get___timg__GetServiceCapabilities(soap, &soap_tmp___timg__GetServiceCapabilities, "-timg:GetServiceCapabilities", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = __timg__GetServiceCapabilities(soap, soap_tmp___timg__GetServiceCapabilities.timg__GetServiceCapabilities, timg__GetServiceCapabilitiesResponse);
	if (soap->error)
		return soap->error;
	soap->encodingStyle = NULL; /* use SOAP literal style */
	soap_serializeheader(soap);
	timg__GetServiceCapabilitiesResponse.soap_serialize(soap);
	if (soap_begin_count(soap))
		return soap->error;
	if ((soap->mode & SOAP_IO_LENGTH))
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || timg__GetServiceCapabilitiesResponse.soap_put(soap, "timg:GetServiceCapabilitiesResponse", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || timg__GetServiceCapabilitiesResponse.soap_put(soap, "timg:GetServiceCapabilitiesResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve___timg__GetImagingSettings(struct soap *soap)
{	struct __timg__GetImagingSettings soap_tmp___timg__GetImagingSettings;
	_timg__GetImagingSettingsResponse timg__GetImagingSettingsResponse;
	timg__GetImagingSettingsResponse.soap_default(soap);
	soap_default___timg__GetImagingSettings(soap, &soap_tmp___timg__GetImagingSettings);
	if (!soap_get___timg__GetImagingSettings(soap, &soap_tmp___timg__GetImagingSettings, "-timg:GetImagingSettings", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = __timg__GetImagingSettings(soap, soap_tmp___timg__GetImagingSettings.timg__GetImagingSettings, timg__GetImagingSettingsResponse);
	if (soap->error)
		return soap->error;
	soap->encodingStyle = NULL; /* use SOAP literal style */
	soap_serializeheader(soap);
	timg__GetImagingSettingsResponse.soap_serialize(soap);
	if (soap_begin_count(soap))
		return soap->error;
	if ((soap->mode & SOAP_IO_LENGTH))
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || timg__GetImagingSettingsResponse.soap_put(soap, "timg:GetImagingSettingsResponse", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || timg__GetImagingSettingsResponse.soap_put(soap, "timg:GetImagingSettingsResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve___timg__SetImagingSettings(struct soap *soap)
{	struct __timg__SetImagingSettings soap_tmp___timg__SetImagingSettings;
	_timg__SetImagingSettingsResponse timg__SetImagingSettingsResponse;
	timg__SetImagingSettingsResponse.soap_default(soap);
	soap_default___timg__SetImagingSettings(soap, &soap_tmp___timg__SetImagingSettings);
	if (!soap_get___timg__SetImagingSettings(soap, &soap_tmp___timg__SetImagingSettings, "-timg:SetImagingSettings", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = __timg__SetImagingSettings(soap, soap_tmp___timg__SetImagingSettings.timg__SetImagingSettings, timg__SetImagingSettingsResponse);
	if (soap->error)
		return soap->error;
	soap->encodingStyle = NULL; /* use SOAP literal style */
	soap_serializeheader(soap);
	timg__SetImagingSettingsResponse.soap_serialize(soap);
	if (soap_begin_count(soap))
		return soap->error;
	if ((soap->mode & SOAP_IO_LENGTH))
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || timg__SetImagingSettingsResponse.soap_put(soap, "timg:SetImagingSettingsResponse", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || timg__SetImagingSettingsResponse.soap_put(soap, "timg:SetImagingSettingsResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve___timg__GetOptions(struct soap *soap)
{	struct __timg__GetOptions soap_tmp___timg__GetOptions;
	_timg__GetOptionsResponse timg__GetOptionsResponse;
	timg__GetOptionsResponse.soap_default(soap);
	soap_default___timg__GetOptions(soap, &soap_tmp___timg__GetOptions);
	if (!soap_get___timg__GetOptions(soap, &soap_tmp___timg__GetOptions, "-timg:GetOptions", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = __timg__GetOptions(soap, soap_tmp___timg__GetOptions.timg__GetOptions, timg__GetOptionsResponse);
	if (soap->error)
		return soap->error;
	soap->encodingStyle = NULL; /* use SOAP literal style */
	soap_serializeheader(soap);
	timg__GetOptionsResponse.soap_serialize(soap);
	if (soap_begin_count(soap))
		return soap->error;
	if ((soap->mode & SOAP_IO_LENGTH))
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || timg__GetOptionsResponse.soap_put(soap, "timg:GetOptionsResponse", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || timg__GetOptionsResponse.soap_put(soap, "timg:GetOptionsResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve___timg__Move(struct soap *soap)
{	struct __timg__Move soap_tmp___timg__Move;
	_timg__MoveResponse timg__MoveResponse;
	timg__MoveResponse.soap_default(soap);
	soap_default___timg__Move(soap, &soap_tmp___timg__Move);
	if (!soap_get___timg__Move(soap, &soap_tmp___timg__Move, "-timg:Move", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = __timg__Move(soap, soap_tmp___timg__Move.timg__Move, timg__MoveResponse);
	if (soap->error)
		return soap->error;
	soap->encodingStyle = NULL; /* use SOAP literal style */
	soap_serializeheader(soap);
	timg__MoveResponse.soap_serialize(soap);
	if (soap_begin_count(soap))
		return soap->error;
	if ((soap->mode & SOAP_IO_LENGTH))
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || timg__MoveResponse.soap_put(soap, "timg:MoveResponse", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || timg__MoveResponse.soap_put(soap, "timg:MoveResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve___timg__Stop(struct soap *soap)
{	struct __timg__Stop soap_tmp___timg__Stop;
	_timg__StopResponse timg__StopResponse;
	timg__StopResponse.soap_default(soap);
	soap_default___timg__Stop(soap, &soap_tmp___timg__Stop);
	if (!soap_get___timg__Stop(soap, &soap_tmp___timg__Stop, "-timg:Stop", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = __timg__Stop(soap, soap_tmp___timg__Stop.timg__Stop, timg__StopResponse);
	if (soap->error)
		return soap->error;
	soap->encodingStyle = NULL; /* use SOAP literal style */
	soap_serializeheader(soap);
	timg__StopResponse.soap_serialize(soap);
	if (soap_begin_count(soap))
		return soap->error;
	if ((soap->mode & SOAP_IO_LENGTH))
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || timg__StopResponse.soap_put(soap, "timg:StopResponse", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || timg__StopResponse.soap_put(soap, "timg:StopResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve___timg__GetStatus(struct soap *soap)
{	struct __timg__GetStatus soap_tmp___timg__GetStatus;
	_timg__GetStatusResponse timg__GetStatusResponse;
	timg__GetStatusResponse.soap_default(soap);
	soap_default___timg__GetStatus(soap, &soap_tmp___timg__GetStatus);
	if (!soap_get___timg__GetStatus(soap, &soap_tmp___timg__GetStatus, "-timg:GetStatus", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = __timg__GetStatus(soap, soap_tmp___timg__GetStatus.timg__GetStatus, timg__GetStatusResponse);
	if (soap->error)
		return soap->error;
	soap->encodingStyle = NULL; /* use SOAP literal style */
	soap_serializeheader(soap);
	timg__GetStatusResponse.soap_serialize(soap);
	if (soap_begin_count(soap))
		return soap->error;
	if ((soap->mode & SOAP_IO_LENGTH))
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || timg__GetStatusResponse.soap_put(soap, "timg:GetStatusResponse", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || timg__GetStatusResponse.soap_put(soap, "timg:GetStatusResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve___timg__GetMoveOptions(struct soap *soap)
{	struct __timg__GetMoveOptions soap_tmp___timg__GetMoveOptions;
	_timg__GetMoveOptionsResponse timg__GetMoveOptionsResponse;
	timg__GetMoveOptionsResponse.soap_default(soap);
	soap_default___timg__GetMoveOptions(soap, &soap_tmp___timg__GetMoveOptions);
	if (!soap_get___timg__GetMoveOptions(soap, &soap_tmp___timg__GetMoveOptions, "-timg:GetMoveOptions", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = __timg__GetMoveOptions(soap, soap_tmp___timg__GetMoveOptions.timg__GetMoveOptions, timg__GetMoveOptionsResponse);
	if (soap->error)
		return soap->error;
	soap->encodingStyle = NULL; /* use SOAP literal style */
	soap_serializeheader(soap);
	timg__GetMoveOptionsResponse.soap_serialize(soap);
	if (soap_begin_count(soap))
		return soap->error;
	if ((soap->mode & SOAP_IO_LENGTH))
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || timg__GetMoveOptionsResponse.soap_put(soap, "timg:GetMoveOptionsResponse", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || timg__GetMoveOptionsResponse.soap_put(soap, "timg:GetMoveOptionsResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve___timg__GetPresets(struct soap *soap)
{	struct __timg__GetPresets soap_tmp___timg__GetPresets;
	_timg__GetPresetsResponse timg__GetPresetsResponse;
	timg__GetPresetsResponse.soap_default(soap);
	soap_default___timg__GetPresets(soap, &soap_tmp___timg__GetPresets);
	if (!soap_get___timg__GetPresets(soap, &soap_tmp___timg__GetPresets, "-timg:GetPresets", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = __timg__GetPresets(soap, soap_tmp___timg__GetPresets.timg__GetPresets, timg__GetPresetsResponse);
	if (soap->error)
		return soap->error;
	soap->encodingStyle = NULL; /* use SOAP literal style */
	soap_serializeheader(soap);
	timg__GetPresetsResponse.soap_serialize(soap);
	if (soap_begin_count(soap))
		return soap->error;
	if ((soap->mode & SOAP_IO_LENGTH))
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || timg__GetPresetsResponse.soap_put(soap, "timg:GetPresetsResponse", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || timg__GetPresetsResponse.soap_put(soap, "timg:GetPresetsResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve___timg__GetCurrentPreset(struct soap *soap)
{	struct __timg__GetCurrentPreset soap_tmp___timg__GetCurrentPreset;
	_timg__GetCurrentPresetResponse timg__GetCurrentPresetResponse;
	timg__GetCurrentPresetResponse.soap_default(soap);
	soap_default___timg__GetCurrentPreset(soap, &soap_tmp___timg__GetCurrentPreset);
	if (!soap_get___timg__GetCurrentPreset(soap, &soap_tmp___timg__GetCurrentPreset, "-timg:GetCurrentPreset", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = __timg__GetCurrentPreset(soap, soap_tmp___timg__GetCurrentPreset.timg__GetCurrentPreset, timg__GetCurrentPresetResponse);
	if (soap->error)
		return soap->error;
	soap->encodingStyle = NULL; /* use SOAP literal style */
	soap_serializeheader(soap);
	timg__GetCurrentPresetResponse.soap_serialize(soap);
	if (soap_begin_count(soap))
		return soap->error;
	if ((soap->mode & SOAP_IO_LENGTH))
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || timg__GetCurrentPresetResponse.soap_put(soap, "timg:GetCurrentPresetResponse", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || timg__GetCurrentPresetResponse.soap_put(soap, "timg:GetCurrentPresetResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve___timg__SetCurrentPreset(struct soap *soap)
{	struct __timg__SetCurrentPreset soap_tmp___timg__SetCurrentPreset;
	_timg__SetCurrentPresetResponse timg__SetCurrentPresetResponse;
	timg__SetCurrentPresetResponse.soap_default(soap);
	soap_default___timg__SetCurrentPreset(soap, &soap_tmp___timg__SetCurrentPreset);
	if (!soap_get___timg__SetCurrentPreset(soap, &soap_tmp___timg__SetCurrentPreset, "-timg:SetCurrentPreset", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = __timg__SetCurrentPreset(soap, soap_tmp___timg__SetCurrentPreset.timg__SetCurrentPreset, timg__SetCurrentPresetResponse);
	if (soap->error)
		return soap->error;
	soap->encodingStyle = NULL; /* use SOAP literal style */
	soap_serializeheader(soap);
	timg__SetCurrentPresetResponse.soap_serialize(soap);
	if (soap_begin_count(soap))
		return soap->error;
	if ((soap->mode & SOAP_IO_LENGTH))
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || timg__SetCurrentPresetResponse.soap_put(soap, "timg:SetCurrentPresetResponse", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || timg__SetCurrentPresetResponse.soap_put(soap, "timg:SetCurrentPresetResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

#if defined(__BORLANDC__)
#pragma option pop
#pragma option pop
#endif

/* End of imageingServer.cpp */
