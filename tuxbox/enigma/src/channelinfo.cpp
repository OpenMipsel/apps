#include <channelinfo.h>
#include <lib/base/i18n.h>
#include <lib/system/init.h>
#include <lib/system/init_num.h>
#include <lib/gui/eskin.h>
#include <lib/gdi/font.h>
#include <lib/dvb/epgcache.h>
#include <lib/dvb/dvbservice.h>
#include <sys/stat.h>
#include <time.h>

eChannelInfo::eChannelInfo( eWidget* parent, const char *deco)
	:eDecoWidget(parent, 0, deco),
	ctime(this), cname(this), copos(this), cdescr(this),
	cgenre(this), cdolby(this), cstereo(this),
	cformat(this), cscrambled(this), eit(0)
{
	foregroundColor=eSkin::getActive()->queryColor("eStatusBar.foreground");
	backgroundColor=eSkin::getActive()->queryColor("eStatusBar.background");
	gFont fn = eSkin::getActive()->queryFont("eChannelInfo");
	gFont ft = fn;
	ft.pointSize+=2;

	cdescr.setFont( fn );
	cdescr.setForegroundColor( foregroundColor );
	cdescr.setBackgroundColor( backgroundColor );
	cdescr.setFlags( RS_FADE /*| eLabel::flagVCenter*/ );

	cgenre.setFont( fn );
	cgenre.setForegroundColor( foregroundColor );
	cgenre.setBackgroundColor( backgroundColor );
	cgenre.setFlags( RS_FADE /*| eLabel::flagVCenter*/ );

	copos.setFont( fn );
	copos.setAlign( eTextPara::dirRight );
	copos.setForegroundColor( foregroundColor );
	copos.setBackgroundColor( backgroundColor );
	copos.setFlags( RS_FADE );

	cname.setFont( ft );
	cname.setForegroundColor( foregroundColor );
	cname.setBackgroundColor( backgroundColor );
	cname.setFlags( RS_FADE );

	ctime.setFont( fn );
	ctime.setForegroundColor( foregroundColor );
	ctime.setBackgroundColor( backgroundColor );
	ctime.setFlags( RS_FADE );

	gPixmap *pm=eSkin::getActive()->queryImage("sselect_dolby");
	cdolby.setPixmap(pm);
	cdolby.pixmap_position = ePoint(0,0);
	cdolby.hide();

	pm = eSkin::getActive()->queryImage("sselect_stereo");
	cstereo.setPixmap(pm);
	cstereo.pixmap_position = ePoint(0,0);
	cstereo.hide();

	pm = eSkin::getActive()->queryImage("sselect_format");
	cformat.setPixmap(pm);
	cformat.pixmap_position = ePoint(0,0);
	cformat.hide();

	pm = eSkin::getActive()->queryImage("sselect_crypt");
	cscrambled.setPixmap(pm);
	cscrambled.pixmap_position = ePoint(0,0);
	cscrambled.hide();
}

const char *eChannelInfo::genresTableShort[256] =
{
	/* 0x0 undefined */    	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
							NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, 

	/* 0x1 Movie */        	_("Movie"),("Thriller"),_("Adventure"),_("SciFi"),_("Comedy"),
							_("Soap"),_("Romance"),_("Serious"),_("Adult"),
							NULL,NULL,NULL,NULL,NULL,NULL,NULL,

	/* 0x2 News */         	_("News"),_("Weather"),_("Magazine"),_("Documentary"),_("Discussion"),
							NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
							NULL,NULL,NULL,

	/* 0x3 Show */         	_("Show"),_("Game Show"),_("Variety"),_("Talk"),
							NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
							NULL,NULL,NULL,NULL,

	/* 0x4 Sports */       	_("Sports"),_("Special Event"),_("Sports Mag."),_("Football"),_("Tennis"),_("Team Sports"),
							_("Athletics"),_("Motor Sports"),_("Water Sports"),_("Winter Sports"),_("Equestrian"),
							_("Martial Sports"),
							NULL,NULL,NULL,NULL,

	/* 0x5 Children */     	_("Children"),_("Pre-School"),_("Age 6-14"),_("Age 10-16"),_("School"),
							_("Cartoons"),
							NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
							NULL,NULL,

	/* 0x6 Music */        	_("Music"),_("Rock/Pop"),_("Classical"),_("Folk"),_("Jazz"),_("Musical"),_("Ballet"),
							NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
							NULL,

	/* 0x7 Culture */      	_("Culture"),_("Perf. Arts"),_("Fine Arts"),_("Religion"),_("Pop. Arts"),_("Literature"),
							_("Film"),_("Experimental"),_("Press"),_("New Media"),_("Art Mag."),_("Fashion"),
							NULL,NULL,NULL,NULL,

	/* 0x8 Social */       	_("Social"),_("Soc. Mag."),_("Economics"),_("Remark. People"),
							NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
							NULL,NULL,NULL,NULL,

	/* 0x9 Education */    	_("Education"),_("Nature"),_("Technology"),_("Medicine"),_("Expeditions"),_("Spiritual"),
							_("Further Ed."),_("Languages"),
							NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,

	/* 0xa Leisure */      	_("Hobbies"),_("Travel"),_("Handicraft"),_("Motoring"),_("Fitness"),_("Cooking"),
							_("Shopping"),_("Gardening"),
							NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,

	/* 0xb Special */      	_("Orig. Lang."),_("B&W"),_("Unpublished"),_("Live"),
							NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
							NULL,NULL,NULL,NULL,

	/* 0xc reserved */     	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
							NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,

	/* 0xd reserved */     	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
							NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,

	/* 0xe reserved */     	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
							NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,

	/* 0xf user defined */ 	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
							NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL
};

void eChannelInfo::ParseEITInfo(EITEvent *e)
{
		name=descr=genre=starttime="";
		cflags=0;
		eString t;
			
		if(e->start_time!=0)
		{
			tm *time=localtime(&e->start_time);
			starttime.sprintf("%02d:%02d", time->tm_hour, time->tm_min);
							
			t.sprintf("  (%dmin)", (int)(e->duration/60));
		}

		if (e->free_CA_mode )
			cflags |= cflagScrambled;
		
		for (ePtrList<Descriptor>::iterator d(e->descriptor); d != e->descriptor.end(); ++d)
		{
//			eDebug(d->toString().c_str());
			Descriptor *descriptor=*d;
			if (descriptor->Tag()==DESCR_SHORT_EVENT)
			{
				ShortEventDescriptor *ss=(ShortEventDescriptor*)descriptor;
				name = ss->event_name;
				descr += ss->text;
				if( (!descr.isNull()) && (descr.c_str()[0])) descr += " ";
			}
      else if (descriptor->Tag()==DESCR_COMPONENT)
			{
				ComponentDescriptor *cd=(ComponentDescriptor*)descriptor;
				
				if( cd->stream_content == 2 && cd->component_type == 5)
					cflags |= cflagDolby;
				else if( cd->stream_content == 2 && cd->component_type == 3)
					cflags |= cflagStereo;
				else if( cd->stream_content == 1 && (cd->component_type == 2 || cd->component_type == 3) )
					cflags |= cflagWide;
			}
			else if(descriptor->Tag()==DESCR_CONTENT)
			{
				ContentDescriptor *cod=(ContentDescriptor*)descriptor;

				for(ePtrList<descr_content_entry_struct>::iterator ce(cod->contentList.begin()); ce != cod->contentList.end(); ++ce)
				{
					if(genresTableShort[ce->content_nibble_level_1*16+ce->content_nibble_level_2])
					{
						if ( !genre.length() )
							genre+=_("GENRE: ");
						genre += gettext( genresTableShort[ce->content_nibble_level_1*16+ce->content_nibble_level_2] );
						genre += " ";
					}
				}
			}
		}
		if(!t.isNull()) name += t;

		int n = 0;
		cname.setText( name );
		cdescr.setText( descr );
		cgenre.setText( genre );
		ctime.setText( starttime );
		n = LayoutIcon(&cdolby, (cflags & cflagDolby), n);
		n = LayoutIcon(&cstereo, (cflags & cflagStereo), n);
		n = LayoutIcon(&cformat, (cflags & cflagWide), n );
		n = LayoutIcon(&cscrambled, (cflags & cflagScrambled), n );
}

void eChannelInfo::closeEIT()
{
	if (eit)
		eit->abort();
}

void eChannelInfo::getServiceInfo( const eServiceReferenceDVB& service )
{
	closeEIT();
	delete eit;
	eit=0;
	
	// eService *service=eServiceInterface::getInstance()->addRef(service);
	
	if (!service.path.size())
	{
		cname.setFlags(RS_FADE);
		cname.resize( eSize(clientrect.width() - ( (clientrect.width() / 8)*2 + 4), (clientrect.height()/3)-2 ));
		int opos=service.getDVBNamespace().get()>>16;
		copos.setText(eString().sprintf("%d.%d\xAF%c", abs(opos / 10), abs(opos % 10), opos>0?'E':'W') );
		EITEvent *e = 0;
		e = eEPGCache::getInstance()->lookupEvent(service);
	//	eDebug(" e = %p", e);	
		if (e)  // data is in cache...
		{
	  	ParseEITInfo(e);
			delete e;
		}
		else  // we parse the eit...
		{
			cname.setText(_("no data for this service avail"));
	
			eDVBServiceController *sapi=eDVB::getInstance()->getServiceAPI();
			if (!sapi)
				return;
			eServiceReferenceDVB &ref = sapi->service;
	
			int type = ((service.getTransportStreamID()==ref.getTransportStreamID())
				&&	(service.getOriginalNetworkID()==ref.getOriginalNetworkID())) ? EIT::tsActual:EIT::tsOther;
	
			eit = new EIT( EIT::typeNowNext, service.getServiceID().get(), type );
			CONNECT( eit->tableReady, eChannelInfo::EITready );
			eit->start();
		}
	} else
	{
		// should be moved to eService
		eString filename=service.path;
		int slice=0;
		struct stat s;
		int filelength=0;
		while (!stat((filename + (slice ? eString().sprintf(".%03d", slice) : eString(""))).c_str(), &s))
		{
			filelength+=s.st_size/1024;
			slice++;
		}
		int i = service.path.rfind("/");
		i++;
		cgenre.setText(eString(_("Filesize: ")) + eString().sprintf("%d MB", filelength/1024));
		cname.setFlags(RS_WRAP);
		cname.resize( eSize(clientrect.width() - (clientrect.width() / 8 + 4), (clientrect.height()/3)*2-2 ));
		cname.setText(eString(_("Filename: "))+ service.path.mid( i, service.path.length()-i ) );
	}
}
	
void eChannelInfo::EITready( int err )
{
//	eDebug("Channelinfo eit ready: %d", err);
	if (eit->ready && !eit->error)
	{
		if ( eit->events.size() )
			ParseEITInfo(eit->events.begin());
	}
	else if ( err == -ETIMEDOUT )
		closeEIT();
}

void eChannelInfo::update( const eServiceReferenceDVB& service )
{
	if (service)
	{
		current = service;
		getServiceInfo(current);
	}
}

void eChannelInfo::clear()
{
	cname.setText("");
	cdescr.setText("");
	cgenre.setText("");
	ctime.setText("");
	copos.setText("");
	cdolby.hide();
	cstereo.hide();
	cformat.hide();
	cscrambled.hide();
}

int eChannelInfo::LayoutIcon(eLabel *icon, int doit, int num )
{
	if( doit )
	{
		int x,y;

		switch(num)
		{
			case 0:
				x=2;
				y=28;
				break;
			case 1:
				x=44;
				y=28;
				break;
			case 2:
				x=2;
				y=50;
				break;
			case 3:
				x=44;
				y=50;
				break;
			default:
				x=0;
				y=0;
				break;
		}
		icon->move(ePoint(x,y));
		icon->show();
		num++;
	}

	return num;

}

void eChannelInfo::redrawWidget(gPainter *target, const eRect& where)
{
	if ( deco )
		deco.drawDecoration(target, ePoint(width(), height()));

	target->line( ePoint(clientrect.left() + clientrect.width()/8 + 1, clientrect.top()),ePoint(clientrect.left() + clientrect.width()/8 + 1,clientrect.bottom()-1));
}

int eChannelInfo::eventHandler(const eWidgetEvent &event)
{
	switch (event.type)
	{
		case eWidgetEvent::changedSize:
		    {
			if (deco)
				clientrect=crect;

			int dx=clientrect.width()/8;
			int dy=clientrect.height()/3;
			ctime.move( ePoint(0,0) );
			ctime.resize( eSize(dx, 36 ));

			cname.move( ePoint( dx + 4, 0 ) );
			cname.resize( eSize( clientrect.width() - (dx + dx + 4), dy+2) );
			
			bool emptygenre=( cgenre.getText().length()==0 );

			cdescr.move( ePoint(dx + 4, dy + 2 ));
			cdescr.resize( eSize(clientrect.width() - (dx + 4), dy+(emptygenre?dy:0)-2 ));

			if(emptygenre){
			    cgenre.move(ePoint(-1,-1));
			    cgenre.resize(eSize(0,0));
			}
			else{
			    cgenre.move( ePoint(dx + 4, dy*2 /*cdescr.getPosition().y() + cdescr.getSize().height())*/) );
			    cgenre.resize( eSize( clientrect.width() - (dx + 4)*2, dy-2 ));
			}

			copos.move( ePoint( clientrect.width() - (dx + 4), 0 ));
			copos.resize( eSize(dx+4, dy-2) );

			cdolby.resize( eSize(25,15) );
			cstereo.resize( eSize(25,15) );
			cformat.resize( eSize(25,15) );
			cscrambled.resize( eSize(25,15) );

			invalidate();
			break;
		    }
		default:
		break;
	}
	return eDecoWidget::eventHandler(event);
}
static eWidget *create_eChannelInfo(eWidget *parent)
{
	return new eChannelInfo(parent);
}

class eChannelInfoSkinInit
{
public:
	eChannelInfoSkinInit()
	{
		eSkin::addWidgetCreator("eChannelInfo", create_eChannelInfo);
	}
	~eChannelInfoSkinInit()
	{
		eSkin::removeWidgetCreator("eChannelInfo", create_eChannelInfo);
	}
};

eAutoInitP0<eChannelInfoSkinInit> init_eChannelInfoSkinInit(eAutoInitNumbers::guiobject, "eChannelInfo");
