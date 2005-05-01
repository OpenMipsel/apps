function setVol(volume)
{
	document.location = "setVolume?volume=" + volume;
	if (window.screen.width < 800)
		setTimeout("reload()", 1000);
	else
	{
		headerUpdateVolumeBar(volume, 0);
		data.location.reload();
	}
}

function toggleMute(xy)
{
	document.location = "setVolume?mute="+xy;
	if (window.screen.width < 800)
		setTimeout("reload()", 1000);
	else
	{
		headerUpdateVolumeBar(data.volume, 0);
		data.location.reload();
	}
}

function switchChannel(xy, bouquet, channel)
{
	if (window.screen.width < 800)
	{
		NewWindow('cgi-bin/zapTo?path='+xy+'&curBouquet='+bouquet+'&curChannel='+channel, 'zap', '1', '1', 'no');
	}
	else
	{
		if (zapMode < 4)
			document.location = "cgi-bin/zapTo?path="+xy+"&curBouquet="+bouquet+"&curChannel="+channel;
		else
			document.location = "?path="+xy+"&mode=zap&zapmode=4&zapsubmode=1";

		if (parent.data.streaming == "1")
			setTimeout("vlc()", 2500);
		setTimeout("parent.data.location.reload()", 2000);
		setTimeout("parent.channavi.location.reload()", 2000);
	}
}

function logging()
{
	parent.body.document.location = "log/debug.html";
}

function remoteControl(box)
{
	if (box == "dbox2")
		NewWindow("showRemoteControl", "RC", "165", "500", "no");
	else
		NewWindow("showRemoteControl", "RC", "1000", "640", "no");
}

function tuneTransponder(transponder)
{
	if (transponder == "none")
	{
		var currentTransponder = document.channelselector.channel.selectedIndex;
		if (currentTransponder >= 0)
		{
			transponder = document.channelselector.channel.options[currentTransponder].value;
			satFinder(transponder);
		}
	}
	else
		satFinder(transponder);
}

function satFinder(transponder)
{
	NewWindow("satFinder?" + transponder, "satfind", "170", "150", "no");
}

function deleteMovie(xy)
{
	if (confirmAction('Do you really want to delete this movie?'))
	{
		if (window.screen.width < 800)
		{
			document.location = "cgi-bin/deleteMovie?ref="+xy;
		}
		else
		{
			var selChannel = document.channelselector.channel.selectedIndex;
			var channel = "";
			if (selChannel >= 0)
			{
				currentChannel = selChannel;
				channel = document.channelselector.channel.options[selChannel].value;
				document.location = "cgi-bin/deleteMovie?ref="+channel;
				setTimeout("reload()", 3000);
			}
			else
				alert("Please select a movie first!");
		}
	}
}

function downloadMovie()
{
	if (window.screen.width >= 800)
	{
		var selChannel = document.channelselector.channel.selectedIndex;
		if (selChannel >= 0)
		{
			currentChannel = selChannel;
			var channel = document.channelselector.channel.options[selChannel].value;
			var pos = channel.indexOf("%2f");
			if (pos >= 0)
			{
				var fileName = channel.substr(pos + 3, channel.length - pos - 3);
				fileName = unescape(fileName);
				document.location = "/rootX/" + fileName;
			}
			else 
				alert("Sorry, no valid filename");
		}
		else
			alert("Please select a movie first!");
	}
	else
		alert("Movie download is not supported on this device.");
}

function recoverMovies()
{
	if (confirmAction('Do you really want to rebuild the movie playlist?'))
	{
		if (window.screen.width < 800)
			document.location = "cgi-bin/recoverRecordings";
		else
		{
			NewWindow("cgi-bin/recoverRecordings", "recoverRecordings", "200", "100", "no", "5000");
			window.setTimeout("document.location.reload()", 2000);
		}
	}
}

function openEPG(xy)
{
	if (xy == undefined)
		xy = "";
	NewWindow('getcurrentepg?type=extended&ref='+xy, 'EPG', screen.width, screen.height, 'yes');
}

function openMultiEPG(xy)
{
	NewWindow('getMultiEPG?ref='+xy, 'MultiEPG', screen.width, screen.height, 'yes');
}

function admin(xy)
{
	NewWindow(xy+'&requester=webif', 'admin', '200', '100', 'no', '3000');
}

function openSI()
{
	NewWindow("cgi-bin/streaminfo", "si", "320", "300", "no");
}

function openChannelInfo()
{
	NewWindow("cgi-bin/channelinfo", "ci", "820", "600", "yes");
}

function DVRrecord(xy)
{
	NewWindow("cgi-bin/record?command="+xy, "record", "200", "100", "no", "5000");
	if (parent.data.updateCycleTime)
		parent.data.location.reload();
	setTimeout("reload()", 500);
}

function startPlugin(xy)
{
	document.location = "cgi-bin/startPlugin?requester=webif&name="+xy;
}

function stopPlugin()
{
	document.location = "cgi-bin/stopPlugin?requester=webif";
}

function sendMessage2TV()
{
	NewWindow("tvMessageWindow", "msg", "780", "150", "no");
}

function selectAudio()
{
	NewWindow("cgi-bin/selectAudio?requester=webif", "audio", "250", "130", "no");
}

function selectSubChannel()
{
	NewWindow("cgi-bin/selectSubChannel", "subchannel", "200", "100", "no");
}

function configSwapFile()
{
	var activate = "off";
	if (swapfileconfig.swap.checked)
		activate = "on";
	document.location = "cgi-bin/setConfigSwapFile?swap="+activate+"&swapfile="+swapfileconfig.swapfile.value;
	setTimeout("reload()", 500);
}

function vlc()
{
	if (parent.data.serviceReference)
		document.location = "video.pls?sref=" + parent.data.serviceReference;
	else
		setTimeout("vlc()", 500);
}
