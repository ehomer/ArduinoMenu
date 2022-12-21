/* -*- C++ -*- */
/********************
Dec. 2022 Eric Homer - ehomer @ alum.mit.edu

Adapted from adafruitGfxOut.h written by Rui Azevedo - ruihfazevedo(@rrob@)gmail.com

Use graphics screens (SparkFun MicroOLED Breakout - library based) as menu output
SparkFun MicroOLED Breakout - https://github.com/sparkfun/SparkFun_Micro_OLED_Arduino_Library

***/
#ifndef RSITE_ARDUINOP_MENU_SFE_MOLED_GFX
	#define RSITE_ARDUINOP_MENU_SFE_MOLED_GFX
	#include <SFE_MicroOLED.h>
	#include "../menuDefs.h"

namespace Menu {

		#define RGB565(r,g,b) ((((r>>3)<<11) | ((g>>2)<<5) | (b>>3)))

	  class sfe_mOledOut:public gfxOut {
	    public:
				MicroOLED& gfx;
				const colorDef<uint16_t> (&colors)[nColors];
		    sfe_mOledOut(MicroOLED& gfx,const colorDef<uint16_t> (&c)[nColors],idx_t* t,panelsList &p,idx_t resX=6,idx_t resY=9)
					:gfxOut(resX,resY,t,p),gfx(gfx),colors(c) {}
					//:gfxOut(gfx.width()/resX,gfx.height()/resY,resX,resY,false),colors(c),gfx(gfx) {}

				size_t write(uint8_t ch) override {return gfx.write(ch);}

				inline uint16_t getColor(colorDefs color=bgColor,bool selected=false,status stat=enabledStatus,bool edit=false) const {
          return memWord(&(stat==enabledStatus?colors[color].enabled[selected+edit]:colors[color].disabled[selected]));
        }

				void setColor(colorDefs c,bool selected=false,status s=enabledStatus,bool e=false) override {
					gfx.setColor(getColor(c,selected,s,e));
				}

				void clearLine(idx_t ln,idx_t panelNr=0,colorDefs color=bgColor,bool selected=false,status stat=enabledStatus,bool edit=false) override {
					const panel p=panels[panelNr];
					gfx.rectFill(p.x*resX,(p.y+ln)*resY,p.maxX()*resX,resY);
		    	//setCursor(0,ln);
		    }
		    void clear() override {
					panels.reset();
					gfx.clear(PAGE);
		    	setCursor(0,0);
					setColor(fgColor);
				}

				void box(idx_t panelNr,idx_t x,idx_t y,idx_t w=1,idx_t h=1,colorDefs c=bgColor,bool selected=false,status stat=enabledStatus,bool edit=false) override {
					const panel p=panels[panelNr];
					gfx.rect((p.x+x)*resX,(p.y+y)*resY,w*resX,h*resY);
				}

				void rect(idx_t panelNr,idx_t x,idx_t y,idx_t w=1,idx_t h=1,colorDefs c=bgColor,bool selected=false,status stat=enabledStatus,bool edit=false) override {
					const panel p=panels[panelNr];
					gfx.rect((p.x+x)*resX,(p.y+y)*resY,w*resX,h*resY);
				}

				void clear(idx_t panelNr) override {
					const panel p=panels[panelNr];
					gfx.rectFill(p.x*resX,p.y*resY,p.w*resX,p.h*resY);
					panels.nodes[panelNr]=NULL;
				}

		    void setCursor(idx_t x,idx_t y,idx_t panelNr=0) override {
					const panel p=panels[panelNr];
					gfx.setCursor((p.x+x)*resX,(p.y+y)*resY+fontMarginY);
				}

				void drawCursor(idx_t ln,bool selected,status stat,bool edit=false,idx_t panelNr=0) override {
					const panel p=panels[panelNr];
					gfx.rect(p.x*resX,(p.y+ln)*resY,maxX()*resX,resY);
				}
	  };

}; //namespace sfe_mOledOut
#endif
