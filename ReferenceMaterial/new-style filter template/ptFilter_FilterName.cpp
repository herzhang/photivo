/*******************************************************************************
**
** Photivo
**
** Copyright (C) YEAR NAME <EMAIL>
**
** This file is part of Photivo.
**
** Photivo is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License version 3
** as published by the Free Software Foundation.
**
** Photivo is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Photivo.  If not, see <http://www.gnu.org/licenses/>.
**
*******************************************************************************/

#include "ptFilter_$FilterName$.h"
/*** Uncomment following line when the filter has a .ui file. ***/
//#include "ui_ptFilter_$FilterName$.h"
#include "ptCfgItem.h"
#include "../ptImage.h"

//------------------------------------------------------------------------------

const QString C$FilterName$Id = "$FilterName$";

const QString CCfgItem1     = "CfgItem1";
const QString CCfgItem2     = "CfgItem2";
const QString CCfgItem3     = "CfgItem3";

//------------------------------------------------------------------------------

ptFilter_$FilterName$::ptFilter_$FilterName$():
  ptFilterBase()
{
  /*** Uncomment as appropriate. ***/
//  FIsSlow = true;
//  FHelpUri = "http://photivo.org/something";
  this->internalInit();
}

//------------------------------------------------------------------------------

ptFilterBase *ptFilter_$FilterName$::create$FilterName$() {
  auto hInstance         = new ptFilter_$FilterName$;
  hInstance->FFilterName = C$FilterName$Id;
  hInstance->FCaption    = tr("GUI toolbox header");
  return hInstance;
}

//------------------------------------------------------------------------------

void ptFilter_$FilterName$::doDefineControls() {
  /***
    * Use short names that indicate what a filter/setting *does*. Be especially careful
      when including probably obscure names of specific algorithms
      (Good: “Wiener sharpen”. Bad: “Wiener” or “Wiener filter”).
    * Use sentence capitalization (“Upper limit”, not “Upper Limit”)
    * Tooltips shall amend the caption with additional and/or more accurate information
      about a setting. *Do not* duplicate the caption! If a tooltip is unnecessary or you
      cannot think of a great one at the moment, leave it empty.
   ***/
  FConfig.initStores(TCfgItemList()                                              //--- Combo: list of entries               ---//
                                                                                 //--- Check: not available                 ---//
    //            Id                       Type                      Default     Min           Max           Step        Decimals, commonConnect, storeable, caption, tooltip
    << ptCfgItem({CCfgItem1,               ptCfgItem::Check,         0,                                                            true, true, tr("Name in GUI"), tr("Tooltip")})
    << ptCfgItem({CCfgItem2,               ptCfgItem::Slider,        0.2,        0.0,          1.0,          0.05,       2,        true, true, tr("Name in GUI"), tr("Tooltip")})
    << ptCfgItem({CCfgItem3,               ptCfgItem::Combo,         0,          QStringList(tr("First list entry"))
                                                                                          << tr("Second list entry")
                                                                                          << tr("Thrid list entry"),               true, true, tr("Name in GUI"),   tr("Tooltip")})
  );
}

//------------------------------------------------------------------------------

bool ptFilter_$FilterName$::doCheckHasActiveCfg() {
  return FConfig.value(CCfgItem2).toDouble() != 0.0f;
}

//------------------------------------------------------------------------------

void ptFilter_$FilterName$::doRunFilter(ptImage *AImage) const {
  AImage->toLab();
  AImage->$FilterName$(FConfig.value(CCfgItem1).toBool(),
                     FConfig.value(CCfgItem2).toDouble(),
                     FConfig.value(CCfgItem3).toInt() );
}

//------------------------------------------------------------------------------

/*** Uncomment following method when the filter has a .ui file. ***/
//QWidget *ptFilter_$FilterName$::doCreateGui() {
//  auto hGuiBody = new QWidget;
//  Ui_$FilterName$Form hForm;

//  hForm.setupUi(hGuiBody);
//  this->initDesignerGui(hGuiBody);

//  return hGuiBody;
//}

//------------------------------------------------------------------------------

RegisterHelper $FilterName$Register(&ptFilter_$FilterName$::create$FilterName$, C$FilterName$Id);

//------------------------------------------------------------------------------
