#include <lib/system/http_dyn.h>
#include <lib/dvb/service.h>
#include <lib/dvb/dvb.h>

extern eString httpUnescape(const eString &string);
extern eString httpEscape(const eString &string);
extern std::map<eString,eString> getRequestOptions(eString opt);
extern eString ref2string(const eServiceReference &r);
extern eServiceReference string2ref(const eString &service);

eString xmlEscape(const eString &string)
{
	eString ret="";
	for (unsigned int i=0; i<string.length(); ++i)
	{
		int c=string[i];
		
		if (c == '&')
			ret+="&amp;";
		else
			ret+=c;
	}
	return ret;
}


static const eString xmlversion="<?xml version=\"1.0\"?>\n";
static inline eString xmlstylesheet(const eString &ss)
{
	return eString("<?xml-stylesheet type=\"text/xsl\" href=\"/xst/") + ss + ".xsl\"?>\n";
}

static eString web_root(eString request, eString dirpath, eString opts, eHTTPConnection *content)
{
	eString ret;
	content->local_header["Content-Type"]="text/xml; charset=utf-8";
	
	ret=xmlversion;
	ret+=xmlstylesheet("services");
	ret+="<services>\n";
	
	for (int i=0; i<10; ++i)
		ret+="<service><name>" + eString().sprintf("Service #%d", i) + "</name></service>\n";
	
	ret+="</services>\n";
	return ret;
}

class eServiceToXml: public Object
{
	eString &result;
	eServiceInterface &iface;
public:
	eServiceToXml(eString &result, eServiceInterface &iface): result(result), iface(iface)
	{
	}
	void addEntry(const eServiceReference &e)
	{
		result+="<service>\n";
		result+="<reference>" + ref2string(e) + "</reference>\n";
		eService *service=iface.addRef(e);
		if (service)
			result+="<name>" + xmlEscape(service->service_name) + "</name>";
		iface.removeRef(e);
		result+="</service>\n";
	}
};

static eString xml_services(eString request, eString dirpath, eString opt, eHTTPConnection *content)
{
	std::map<eString,eString> opts=getRequestOptions(opt);
	eString spath=opts["path"];

	eServiceInterface *iface=eServiceInterface::getInstance();

	if (!iface)
		return "n/a\n";

	eString current;

	unsigned int pos;
	if ((pos=spath.rfind(';')) != eString::npos)
	{
		current=spath.mid(pos+1);
		spath=spath.left(pos);
	} else
	{
		current=spath;
		spath="";
	}
	
	eServiceReference current_service=string2ref(current);
	
	eDebug("current_service: %s", current_service.path.c_str());

	eString res;
	
	eServiceToXml conv(res, *iface);

	Signal1<void,const eServiceReference&> signal;
	signal.connect(slot(conv, &eServiceToXml::addEntry));

	res=xmlversion;
	res+=xmlstylesheet("services");
	res+="<services>\n";

	iface->enterDirectory(current_service, signal);
	iface->leaveDirectory(current_service);
	
	res+="</services>\n";

	return res;
}

void ezapInitializeWeb(eHTTPDynPathResolver *dyn_resolver)
{
	dyn_resolver->addDyn("GET", "/dyn2/", web_root);
	dyn_resolver->addDyn("GET", "/dyn2/services", xml_services);
}
