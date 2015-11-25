/*	File:		IconMenus.c	Contains:		Written by: 		Copyright:	Copyright � 1984-1999 by Apple Computer, Inc., All Rights Reserved.				You may incorporate this Apple sample source code into your program(s) without				restriction. This Apple sample source code has been provided "AS IS" and the				responsibility for its operation is yours. You are not permitted to redistribute				this Apple sample source code as "Apple sample source code" after having made				changes. If you're going to re-distribute the source, we require that you make				it clear in the source that the code was descended from Apple sample source				code, but that you've made changes.	Change History (most recent first):				7/9/1999	Karl Groethe	Updated for Metrowerks Codewarror Pro 2.1				*/#include "IconMenus.h"#include <MacTypes.h>#include <Menus.h>#include <QDOffscreen.h>#include <Windows.h>#include <ToolUtils.h>#include <Devices.h>#include <Sound.h>WindowPtr bullseyeWindow;GWorldPtr	GgwPtr;static void enable(MenuHandle menu, short item, Boolean ok);void SetUpMenus();void AdjustMenus();void HandleMenu (long mSelect);MenuHandle	appleMenu, fileMenu, editMenu;enum	{	appleID = 1,	fileID,	editID,	widthID	};enum	{	openItem = 1,	closeItem,	quitItem = 4	};/**** * SetUpMenus() * *	Set up the menus. Normally, we�d use a resource file, but *	for this example we�ll supply �hardwired� strings. * ****/void SetUpMenus(){	InsertMenu(appleMenu = NewMenu(appleID, "\p\024"), 0);	InsertMenu(fileMenu = NewMenu(fileID, "\pFile"), 0);	InsertMenu(editMenu = NewMenu(editID, "\pEdit"), 0);	DrawMenuBar();	AppendResMenu(appleMenu, 'DRVR');	AppendMenu(fileMenu, "\pOpen/O;Close/W;(-;Quit/Q");	AppendMenu(editMenu, "\pUndo/Z;(-;Cut/X;Copy/C;Paste/V;Clear");}/* end SetUpMenus *//**** *  AdjustMenus() * *	Enable or disable the items in the Edit menu if a DA window *	comes up or goes away. Our application doesn't do anything with *	the Edit menu. * ****/void AdjustMenus(){	register WindowPeek wp = (WindowPeek) FrontWindow();	short kind = wp ? wp->windowKind : 0;	Boolean DA = kind < 0;		enable(editMenu, 1, DA);	enable(editMenu, 3, DA);	enable(editMenu, 4, DA);	enable(editMenu, 5, DA);	enable(editMenu, 6, DA);		enable(fileMenu, openItem, !((WindowPeek) bullseyeWindow)->visible);	enable(fileMenu, closeItem, DA || ((WindowPeek) bullseyeWindow)->visible);}static void enable(MenuHandle menu, short item, Boolean ok){	if (ok)		EnableItem(menu, item);	else		DisableItem(menu, item);}/***** * HandleMenu(mSelect) * *	Handle the menu selection. mSelect is what MenuSelect() and *	MenuKey() return: the high word is the menu ID, the low word *	is the menu item * *****/void HandleMenu (long mSelect){	int			menuID = HiWord(mSelect);	int			menuItem = LoWord(mSelect);	Str255		name;	GrafPtr		savePort;	WindowPeek	frontWindow;		switch (menuID)	  {	  case	appleID:		GetPort(&savePort);		GetMenuItemText(appleMenu, menuItem, name);		OpenDeskAcc(name);		SetPort(savePort);		break;		  case	fileID:		switch (menuItem)		  {		  case	openItem:			ShowWindow(bullseyeWindow);			SelectWindow(bullseyeWindow);			break;  				  					  case	closeItem:			if ((frontWindow = (WindowPeek) FrontWindow()) == 0L)			  break;			  			if (frontWindow->windowKind < 0)			  CloseDeskAcc(frontWindow->windowKind);			else if (frontWindow = (WindowPeek) bullseyeWindow)			  HideWindow(bullseyeWindow);  				  	break;  				  			  case	quitItem:		  	DisposeGWorld(GgwPtr);			ExitToShell();			break;		  }		break;  					  case	editID:		if (!SystemEdit(menuItem-1))		  SysBeep(5);		break;			  }}/* end HandleMenu */