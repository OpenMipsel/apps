#include <lib/base/estring.h>
#include <lib/gui/textinput.h>
#include <lib/gui/guiactions.h>
#include <lib/gui/numberactions.h>
#include <lib/gui/eskin.h>
#include <lib/gui/ewindow.h>
#include <lib/system/init.h>
#include <lib/system/econfig.h>
#include <lib/gdi/font.h>

eTextInputField::eTextInputField( eWidget *parent, eLabel *descr, const char *deco )
	:eButton( parent, descr, 1, deco), maxChars(0), lastKey(-1), editMode(false), nextCharTimer(eApp),
    useableChars("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
//						 " +-.,:?!\"';_*/()<=>%#@&"),
             "ĞÑÒÓÔÕÖ×ØÙÚÛÜİŞßàáâãäåæçèéêëìíîï°±²³´µ¶·¸¹º»¼½¾¿ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏ"
						 " +-.,:?!\"';_*/()<=>%#@&"), capslock(0)
{
	if ( eConfig::getInstance()->getKey("/ezap/rc/TextInputField/nextCharTimeout", nextCharTimeout ) )
		nextCharTimeout=0;
	CONNECT( nextCharTimer.timeout, eTextInputField::nextChar );
	addActionMap(&i_numberActions->map);
	flags=0;
	align=eTextPara::dirLeft;
}

void eTextInputField::updated()
{
	unsigned char c[4096];
	strcpy((char *)c,isotext.c_str());
		
	text=convertDVBUTF8(c,strlen((char*)c));
	eLabel::invalidate();
	drawCursor();
}

void eTextInputField::nextChar()
{
	if ( curPos+1 < maxChars )
	{
		++curPos;
		if ( curPos > isotext.length()-1 )
			isotext+=' ';
		updated();
	}
	lastKey=-1;
}

//		"abc2ABC",   // 2
//		"def3DEF",   // 3
//		"ghi4GHI»",   // 4
//		"jkl5JKL",   // 5
//		"mno6MNO",   // 6
//		"pqrs7PQRS", // 7
//		"tuv8TUV",   // 8
//		"wxyz9WXYZ",   // 9

const char *keys[2][12] = {
// lowercase
	  { 	"+0-.,:?!\"';_",     // 0
		" 1#@&/()<=>%",      // 1
		"abc2ĞÑÒÓ",   // 2
		"def3ÔÕÖ×",   // 3
		"ghi4ØÙÚÛ",   // 4
		"jkl5ÜİŞß",   // 5
		"mno6àáâã",   // 6
		"pqrs7äåæç",  // 7
		"tuv8èéêë",   // 8
		"wxyz9íîï",   // 9
		"*/()<=>%",          // <
		"#@&`" },            // >
// uppercase
	  { 	"+0-.,:?!\"';_",     // 0
		" 1#@&/()<=>%",      // 1
		"ABC2°±²³",   // 2
		"DEF3´µ¶·",   // 3
		"GHI4¸¹º»",   // 4
		"JKL5¼½¾¿",   // 5
		"MNO6ÀÁÂÃ",   // 6
		"PQRS7ÄÅÆÇ",  // 7
		"TUV8ÈÉÊË",   // 8
		"WXYZ9ÍÎÏ",   // 9
		"*/()<=>%",          // <
		"#@&`" }             // >
};

void eTextInputField::setUseableChars( const char* uchars )
{
	useableChars=uchars;
}

void eTextInputField::setNextCharTimeout( unsigned int newtimeout )
{
	nextCharTimeout=newtimeout;
}

void eTextInputField::drawCursor()
{
	eRect rc;
	rc.setTop(crect.bottom()-4);
	rc.setHeight( 3 );
	if ( para )
	{
		if ( isotext.length() )  // text exist?
		{
			if ( isotext.length() > curPos) // before or on the last char?
			{
				const eRect &bbox = para->getGlyphBBox(curPos);
				if ( !bbox.width() )  // Space
				{
					if (curPos)  // char before space?
					{
						const eRect &bbBefore = para->getGlyphBBox(curPos-1);
						rc.setLeft( bbBefore.right()+2 );
					}
					if ( isotext.length() > curPos+1) // char after space ?
					{
						const eRect &bbAfter = para->getGlyphBBox(curPos+1);
						rc.setRight( bbAfter.left()-2 );
					}
					else  // no char behind Space
						rc.setWidth( 10 );
				}
				else
				{
					rc.setLeft( bbox.left() );
					rc.setWidth( bbox.width() );
				}
			}
			else // we are behind the last character
			{
				const eRect &bbox = para->getGlyphBBox(isotext.length()-1);
				rc.setLeft( bbox.right() + ( ( curPos-isotext.length() ) * 10 ) + 2 );
				rc.setWidth( 10 );
			}
		}
		else  //  no one character in text
		{
			rc.setLeft( 2 );
			rc.setWidth( 10 );
		}
		rc.moveBy( (deco_selected?crect_selected.left():crect.left())+1, 0 );
		gPainter *painter = getPainter( deco_selected?crect_selected:crect );
		painter->setForegroundColor( getForegroundColor() );
		painter->setBackgroundColor( getBackgroundColor() );
		painter->clip( rc );
		painter->fill( rc );
		painter->clippop();
		if(capslock){
			rc.setTop(crect.top());
			rc.setHeight( 3 );
			painter->clip( rc );
			painter->fill( rc );
			painter->clippop();
		}
		delete painter;
	}
}

int eTextInputField::eventHandler( const eWidgetEvent &event )
{
	isotext=convertUTF8DVB(text);
	switch (event.type)
	{
		case eWidgetEvent::changedText:
			if ( maxChars < isotext.length() )
				maxChars = isotext.length();
			return eButton::eventHandler( event );
		break;
		case eWidgetEvent::evtAction:
		{
			int key = -1;
			if ( event.action == &i_cursorActions->capslock && editMode){
				capslock^=1;
				eLabel::invalidate();
				drawCursor();
			}
			else if ( (event.action == &i_cursorActions->up || event.action == &i_cursorActions->down) && editMode)
			{
				nextCharTimer.stop();
				const char *pc1=useableChars.c_str();
				if ( curPos>=isotext.length() ){
					if (event.action == &i_cursorActions->down){
						while(*pc1)pc1++;
					  pc1--;
					}
				  isotext += *pc1;
				}
				else{
					const char *pc2=strchr( pc1, isotext[curPos] );
					if(!pc2 || !pc2[0]){ pc2=pc1; }
					if(event.action == &i_cursorActions->up){
						pc2++;
						if(!pc2[0])pc2=pc1;
					}
					else{
						if(pc2==pc1)
						{
							while(*pc2)pc2++;
						}
						pc2--;
					}
					isotext[curPos] = *pc2;
				}
				updated();
			}
			else if (event.action == &i_cursorActions->insertchar && editMode)
			{
				if ( isotext.length()<maxChars )
				{
					isotext.insert( curPos, " ");
					updated();
				}
			}
			else if (event.action == &i_cursorActions->deletechar && editMode)
			{
				if ( isotext.length() )
				{
					isotext.erase( curPos, 1 );
//					eDebug("curPos=%d, length=%d", curPos, text.length() );
					if ( isotext.length() == curPos )
					{
//						eDebug("curPos--");
						curPos--;
					}
					updated();
				}
			}
			else if (event.action == &i_cursorActions->left && editMode )
			{
				nextCharTimer.stop();
				if ( curPos > 0 )
				{
					--curPos;
					lastKey=-1;
					updated();
				}
			}
			else if (event.action == &i_cursorActions->right && editMode)
			{
				nextCharTimer.stop();
				nextChar();
			}
			else if (event.action == &i_cursorActions->ok)
			{
				nextCharTimer.stop();
				if ( editMode )
				{
					setHelpText(oldHelpText);
					if(isotext.length()>0)
						while ( isotext[isotext.length()-1] == ' ' )
							isotext.erase( isotext.length()-1 );

					updated();
					eButton::invalidate();  // remove the underline
					editMode=false;
					eWindow::globalCancel(eWindow::ON);
				}
				else
				{
					oldHelpText=helptext;
					oldText=text;
					setHelpText(_("press ok to leave edit mode, yellow=capslock"));
					editMode=true;
					curPos=0;
					drawCursor();
					eWindow::globalCancel(eWindow::OFF);
				}
			}
			else if ( editMode && event.action == &i_cursorActions->cancel)
			{
				nextCharTimer.stop();
				editMode=false;
				setText(oldText,false);
				invalidate();
				eWindow::globalCancel(eWindow::ON);
			}
			else if (event.action == &i_numberActions->key0)
				key=0;
			else if (event.action == &i_numberActions->key1)
				key=1;
			else if (event.action == &i_numberActions->key2)
				key=2;
			else if (event.action == &i_numberActions->key3)
				key=3;
			else if (event.action == &i_numberActions->key4)
				key=4;
			else if (event.action == &i_numberActions->key5)
				key=5;
			else if (event.action == &i_numberActions->key6)
				key=6;
			else if (event.action == &i_numberActions->key7)
				key=7;
			else if (event.action == &i_numberActions->key8)
				key=8;
			else if (event.action == &i_numberActions->key9)
				key=9;
			else if (event.action == &i_numberActions->keyExt1)
				key=10;
			else if (event.action == &i_numberActions->keyExt2)
				key=11;
			else
				return eButton::eventHandler( event );
			if ( key != lastKey && nextCharTimer.isActive() )
			{
				nextCharTimer.stop();
				nextChar();
			}
			if ( editMode && key != -1 )
			{
				char newChar = 0;
				
				if ( key == lastKey )
				{
					char *oldkey = strchr( keys[capslock][key], isotext[curPos] );
					newChar = oldkey?keys[capslock][key][oldkey-keys[capslock][key]+1]:0;
				}

				if (!newChar)
				{
					newChar = keys[capslock][key][0];
				}
				char testChar = newChar;
				do
				{
					if ( strchr( useableChars.c_str(), newChar ) ) // char useable?
					{
						if ( curPos == isotext.length() )
							text += newChar;
						else
							isotext[curPos] = newChar;
						updated();
						if(nextCharTimeout)
							nextCharTimer.start(nextCharTimeout,true);
						break;
					}
					else
					{
						nextCharTimer.stop();
						char *oldkey = strchr( keys[capslock][key], newChar );
						newChar=oldkey?keys[capslock][key][oldkey-keys[capslock][key]+1]:0;
						if (!newChar)
							newChar=keys[capslock][key][0];
					}
				}
				while( newChar != testChar );  // all chars tested.. and no char is useable..
				lastKey=key;
			}
		}
		break;

		default:
			return eButton::eventHandler( event );
		break;
	}
	return 1;
}

void eTextInputField::redrawWidget( gPainter *target, const eRect &area )
{
	eButton::redrawWidget( target, area );
}

static eWidget *create_eTextInputField(eWidget *parent)
{
	return new eTextInputField(parent);
}

class eTextInputFieldSkinInit
{
public:
	eTextInputFieldSkinInit()
	{
		eSkin::addWidgetCreator("eTextInputField", create_eTextInputField);
	}
	~eTextInputFieldSkinInit()
	{
		eSkin::removeWidgetCreator("eTextInputField", create_eTextInputField);
	}
};

eAutoInitP0<eTextInputFieldSkinInit> init_eTextInputFieldSkinInit(25, "eTextInputField");

