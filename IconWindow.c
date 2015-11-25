/*	File:		IconWindow.c	Contains:		Written by: 		Copyright:	Copyright � 1984-1999 by Apple Computer, Inc., All Rights Reserved.				You may incorporate this Apple sample source code into your program(s) without				restriction. This Apple sample source code has been provided "AS IS" and the				responsibility for its operation is yours. You are not permitted to redistribute				this Apple sample source code as "Apple sample source code" after having made				changes. If you're going to re-distribute the source, we require that you make				it clear in the source that the code was descended from Apple sample source				code, but that you've made changes.	Change History (most recent first):				7/9/1999	Karl Groethe	Updated for Metrowerks Codewarror Pro 2.1				*/#include "IconWindow.h"#include <Windows.h>#include <MacTypes.h>#include <Resources.h>WindowPtr bullseyeWindow;Rect		dragRect;Rect		windowBounds = { 40, 40, 300, 500 };Rect		circleStart = {10, 10, 100, 100};int			width = 5;/**** * SetUpWindow() * *	Create the Bullseye window, and open it. * ****/void SetUpWindow(){	dragRect = qd.screenBits.bounds;		bullseyeWindow = NewWindow(0L, &windowBounds, "\pBullseye", true, noGrowDocProc,(WindowPtr)-1L, true, 0);	SetPort(bullseyeWindow);}/* end SetUpWindow */#define BYTESIZE 8#define WIDTHHEIGHT 16GWorldPtr	GgwPtr = NULL;Rect	PlotRect = {10,200,26,216};Boolean RectNotInGrayRgn(Rect *r){	RgnHandle rgn = GetGrayRgn();	Rect	IRect,wRect;		IRect = (**rgn).rgnBBox;	SectRect(&IRect,r,&wRect);		return EmptyRect(&wRect);}void FindNewDevice(WindowPtr	wind){	Rect	gRect,lRect,wRect;	GDHandle	aDevice;	GDHandle	dominateDevice = NULL;	long		gArea,cArea;	GrafPtr		gp;		GetPort(&gp);	SetPort(wind);		gRect = PlotRect;	LocalToGlobal((Point *)&gRect.top);	LocalToGlobal((Point *)&gRect.bottom);		aDevice = GetDeviceList();	gArea = cArea = 0;	while ( aDevice ) {		lRect = (**aDevice).gdRect;		SectRect(&lRect,&gRect,&wRect);				if ( !EmptyRect(&wRect) ) {			cArea = (long)(wRect.right - wRect.left) * (long)(wRect.bottom - wRect.top);					if ( cArea > gArea ) {				gArea = cArea;				dominateDevice = aDevice;			}		}				aDevice = GetNextDevice(aDevice);	}		if ( GgwPtr && dominateDevice ) {		if ( (**((**dominateDevice).gdPMap)).pixelSize != (**(GgwPtr->portPixMap)).pixelSize)			InvalRect(&PlotRect);	}	SetPort(gp);}						void MakeANewGWorld(Rect *r){	Rect		localR;	Handle		h;	QDErr		err;	short		i;	Ptr			p,tp;	short		depth;			localR = *r;		/* 	   by passing a global rectangle we force NewGWorld to determine the resolution	   of the device the the majority of the rectangle lies on.  Then we can use the pixel	   depth to determine which icl or ics to get	*/	LocalToGlobal((Point *)&localR.top);	LocalToGlobal((Point *)&localR.bottom);		if ( RectNotInGrayRgn(&localR) )		return;	if ( !GgwPtr ) {		if ( err = NewGWorld(&GgwPtr,0,&localR,NULL,NULL,noNewDevice) )			return;	}	else	if ( err = UpdateGWorld(&GgwPtr,0,&localR,NULL,NULL,0) )		   return;			/* now grab the right depth of icl or ics */		depth = (**(GgwPtr->portPixMap)).pixelSize; 		switch ( depth ) {			 			case 2: SetRect(&localR,0,0,16,16);					DisposeGWorld(GgwPtr);					if ( err = NewGWorld(&GgwPtr,1,&localR,NULL,NULL,0) )		   				return; 			case 1: h = GetResource('ics#',128);					depth = 1;					break;			case 4:	h = GetResource('ics4',128);					break;			case 8:	h = GetResource('ics8',128);					break;			default: h = GetResource('ics8',128);					 depth = 8;					 SetRect(&localR,0,0,16,16);					 DisposeGWorld(GgwPtr);					 if ( err = NewGWorld(&GgwPtr,8,&localR,NULL,NULL,0) )		   				return; 					 break;		}								/* we will be working with the pixel data directly so lock them */		p = GetPixBaseAddr(GgwPtr->portPixMap);		if ( p ) {			HLock(h);			tp = *h;						/* 				because NewGWorld usually adds a long word of padding so that				it can more easily keep the pixel map long word aligned we				can't just BlockMove the data directly.  Instead we have to				move one row from our source and then advance the pointer of the				destination to the next row			*/			for (  i = 0; i < WIDTHHEIGHT; i++ ) { 				BlockMove(tp,p,(WIDTHHEIGHT / BYTESIZE) * depth);		/* move a row */				/*				  now advance the destination pointer to the next row.  Don't				  forget to and off he high bit which marks this as a pixelmap				 */				p += ((**(GgwPtr->portPixMap)).rowBytes ) & 0x7fff;				tp += (WIDTHHEIGHT / BYTESIZE) * depth; /*move the source pointer to the next row */			}			HUnlock(h);			ReleaseResource(h);		}		}void DrawIcResources(){	Rect	r1;						r1 = PlotRect;	MakeANewGWorld(&r1);			if ( GgwPtr ) {		r1 = (**(GgwPtr->portPixMap)).bounds;		r1.right = r1.left + 16;		CopyBits((BitMap *)(*(GgwPtr->portPixMap)),&bullseyeWindow->portBits,					&(r1),&PlotRect,srcCopy,NULL);	}}		/***** * DrawBullseye() * *	Draws the bullseye. * *****/void DrawBullseye(short	active){	#pragma unused(active)	SetPort(bullseyeWindow);	EraseRect(&bullseyeWindow->portRect);				DrawIcResources();}/* end DrawBullseye */