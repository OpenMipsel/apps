/*
 * $Id: getservices.cpp,v 1.67.2.1 2003/02/18 15:16:46 alexw Exp $
 */

#include <stdio.h>

#include <zapit/bouquets.h>
#include <zapit/channel.h>
#include <zapit/debug.h>
#include <zapit/frontend.h>
#include <zapit/getservices.h>
#include <zapit/settings.h>
#include <zapit/xmlinterface.h>

extern std::map <uint32_t, transponder> transponders;
extern tallchans allchans;

void ParseTransponders(xmlNodePtr node, const uint8_t DiSEqC)
{
	t_transport_stream_id transport_stream_id;
	t_original_network_id original_network_id;
	FrontendParameters feparams;
	uint8_t polarization = 0;
	uint8_t tmp;

	/* FIXME: get inversion from services list */
	feparams.Inversion = INVERSION_AUTO;

	/* read all transponders */
	while ((node = xmlGetNextOccurence(node, "transponder")) != NULL)
	{
		/* common */
		transport_stream_id = xmlGetNumericAttribute(node, "id", 16);
		original_network_id = xmlGetNumericAttribute(node, "onid", 16);
		feparams.Frequency = xmlGetNumericAttribute(node, "frequency", 0);

		/* cable */
		if (DiSEqC == 0xFF)
		{
			feparams.u.qam.SymbolRate = xmlGetNumericAttribute(node, "symbol_rate", 0);
			sscanf(xmlGetAttribute(node, "fec_inner"), "%hhu", &tmp);
			feparams.u.qam.FEC_inner = CFrontend::getFEC(tmp);
			sscanf(xmlGetAttribute(node, "modulation"), "%hhu", &tmp);
			feparams.u.qam.QAM = CFrontend::getModulation(tmp);
		}

		/* satellite */
		else
		{
			feparams.u.qpsk.SymbolRate = xmlGetNumericAttribute(node, "symbol_rate", 0);
			sscanf(xmlGetAttribute(node, "fec_inner"), "%hhu", &tmp);
			feparams.u.qpsk.FEC_inner = CFrontend::getFEC(tmp);
			polarization = xmlGetNumericAttribute(node, "polarization", 0);
		}

		/* add current transponder to list */
		transponders.insert
		(
			std::pair <uint32_t, transponder>
			(
				(transport_stream_id << 16) | original_network_id,
				transponder
				(
					transport_stream_id,
					feparams,
					polarization,
					DiSEqC,
					original_network_id
				)
			)
		);

		/* read channels that belong to the current transponder */
		ParseChannels(node->xmlChildrenNode, transport_stream_id, original_network_id, DiSEqC);

		/* hop to next transponder */
		node = node->xmlNextNode;
	}

	return;
}

void ParseChannels(xmlNodePtr node, const t_transport_stream_id transport_stream_id, const t_original_network_id original_network_id, const unsigned char DiSEqC)
{
	t_service_id service_id;
	std::string  name;
	uint8_t      service_type;

	while ((node = xmlGetNextOccurence(node, "channel")) != NULL)
	{
		service_id = xmlGetNumericAttribute(node, "service_id", 16);
		name = xmlGetAttribute(node, "name");
		service_type = xmlGetNumericAttribute(node, "service_type", 16);

		switch (service_type) {
		case DIGITAL_TELEVISION_SERVICE:
		case NVOD_REFERENCE_SERVICE:
		case NVOD_TIME_SHIFTED_SERVICE:
		case DIGITAL_RADIO_SOUND_SERVICE:
			allchans.insert
			(
				std::pair <t_channel_id, CZapitChannel>
				(
					CREATE_CHANNEL_ID,
					CZapitChannel
					(
						name,
						service_id,
						transport_stream_id,
						original_network_id,
						service_type,
						DiSEqC
					)
				)
			);

			break;

		default:
			break;
		}

		node = node->xmlNextNode;
	}

	return;
}

void FindTransponder(xmlNodePtr search)
{
	uint8_t DiSEqC;

	while (search)
	{
		if (!(strcmp(xmlGetName(search), "cable")))
			DiSEqC = 0xff;

		else if (!(strcmp(xmlGetName(search), "sat")))
			DiSEqC = xmlGetNumericAttribute(search, "diseqc", 0);

		else {
			search = search->xmlNextNode;
			continue;
		}

		INFO("going to parse dvb-%c provider %s", xmlGetName(search)[0], xmlGetAttribute(search, "name"));
		ParseTransponders(search->xmlChildrenNode, DiSEqC);
		search = search->xmlNextNode;
	}
}

int LoadServices(void)
{
	xmlDocPtr parser = parseXmlFile(string(SERVICES_XML));

	if (parser == NULL)
		return -1;

	FindTransponder(xmlDocGetRootElement(parser)->xmlChildrenNode);
	xmlFreeDoc(parser);
	return 0;
}

