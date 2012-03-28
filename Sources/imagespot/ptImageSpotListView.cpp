/*******************************************************************************
**
** Photivo
**
** Copyright (C) 2011-2012 Bernd Schoeler <brjohn@brother-john.net>
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

#include <memory>

#include "ptImageSpotListView.h"
#include "ptImageSpotModel.h"
#include "ptImageSpot.h"
#include "../ptTheme.h"
#include "../ptImage.h"
#include "../ptSettings.h"
#include "../ptProcessor.h"
#include "../ptGroupBox.h"

extern ptTheme     *Theme;
extern ptSettings  *Settings;
extern ptProcessor *TheProcessor;

extern void Update(short Phase,
                   short SubPhase       = -1,
                   short WithIdentify   = 1,
                   short ProcessorMode  = ptProcessorMode_Preview);
extern void UpdatePreviewImage(const ptImage* ForcedImage   = NULL,
                               const short    OnlyHistogram = 0);

//==============================================================================

ptImageSpotListView::ptImageSpotListView(QWidget *AParent,
                                         ptImageSpot::PCreateSpotFunc ASpotCreator)
: QListView(AParent),
  FAppendOngoing(false),
  FInteractionOngoing(false),
  FModel(nullptr),
  FSpotCreator(ASpotCreator)
{
  setStyle(Theme->style());
  setStyleSheet(Theme->stylesheet());

  setEditTriggers(QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed);
  setSelectionMode(QAbstractItemView::SingleSelection);

//  setDragDropMode(QAbstractItemView::InternalMove);
  setDragEnabled(false);
//  setDropIndicatorShown(true);
//  setAcceptDrops(true);
}

//==============================================================================

ptImageSpotListView::~ptImageSpotListView() {}

//==============================================================================

void ptImageSpotListView::setInteractionOngoing(const bool AOngoing) {
  FInteractionOngoing = AOngoing;

  if (FInteractionOngoing && FModel->rowCount() > 0) {
    // Pipe stops *before* spot processing to provide a clean basis for the quick
    // preview in interactive mode. To make present spots immediately visibe when
    // entering the interaction, we trigger spot processing now.
    UpdatePreview();
  }
}

//==============================================================================

void ptImageSpotListView::setModel(ptImageSpotModel *model) {
  FModel = model;
  QListView::setModel(model);
}

//==============================================================================

void ptImageSpotListView::UpdatePreview() {
  if (FInteractionOngoing) {
    // We’re in interactive mode: only recalc spots
    std::unique_ptr<ptImage> hImage(new ptImage);
    hImage->Set(TheProcessor->m_Image_AfterLocalEdit);
    hImage->RGBToLch();
    FModel->RunFiltering(hImage.get());
    hImage->LchToRGB(Settings->GetInt("WorkColor"));
    UpdatePreviewImage(hImage.get());

  } else {
    // not in interactive mode: run pipe
    Update(ptProcessorPhase_LocalEdit);
  }
}

//==============================================================================

void ptImageSpotListView::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight) {
  if (FModel->lastChangedRole() == Qt::CheckStateRole) {
    UpdatePreview();
  }
  UpdateToolActiveState();
  QListView::dataChanged(topLeft, bottomRight);
}

//==============================================================================

void ptImageSpotListView::keyPressEvent(QKeyEvent *event) {
  if (event->key() == Qt::Key_Delete && event->modifiers() == Qt::NoModifier) {
    if (currentIndex().isValid()) this->deleteSpot();

  } else if (event->key() == Qt::Key_Down && event->modifiers() == Qt::ControlModifier) {
    moveSpotDown();

  } else if (event->key() == Qt::Key_Up && event->modifiers() == Qt::ControlModifier) {
    moveSpotUp();

  } else {
    // Base class takes care of standard stuff like cursor keys
    QListView::keyPressEvent(event);
  }
}

//==============================================================================

void ptImageSpotListView::UpdateToolActiveState() {
  // Copied from ptMain::Standard_CB_SetAndRun().
  QWidget* CurrentControl = this->parentWidget();
  ptGroupBox* CurrentTool = dynamic_cast<ptGroupBox*>(CurrentControl);

  while (CurrentTool == NULL) {
    CurrentControl = CurrentControl->parentWidget();
    CurrentTool = dynamic_cast<ptGroupBox*>(CurrentControl);
  }

  CurrentTool->SetActive(FModel->hasEnabledSpots());
}

//==============================================================================

void ptImageSpotListView::deleteSpot() {
  FModel->removeRows(currentIndex().row(), 1, QModelIndex());
  UpdatePreview();
  emit rowChanged(currentIndex());
}

//==============================================================================

void ptImageSpotListView::setAppendMode(const bool AAppendOngoing) {
  FAppendOngoing = AAppendOngoing;
}

//==============================================================================

void ptImageSpotListView::moveSpotDown() {
  int hNewRow = FModel->MoveRow(currentIndex(), +1);
  if (hNewRow > -1) {
    this->setCurrentIndex(FModel->index(hNewRow, 0));
    UpdatePreview();
  }
}

//==============================================================================

void ptImageSpotListView::moveSpotUp() {
  int hNewRow = FModel->MoveRow(currentIndex(), -1);
  if (hNewRow > -1) {
    this->setCurrentIndex(FModel->index(hNewRow, 0));
    UpdatePreview();
  }
}

//==============================================================================

void ptImageSpotListView::processCoordinates(const QPoint &APos) {
  if (FAppendOngoing) {
    // append new spot after append-spot button was clicked
    ptImageSpot *hSpot = FSpotCreator();
    hSpot->setPos(APos.x(), APos.y());
    hSpot->setName(tr("Spot"));

    FModel->appendSpot(hSpot);
    this->setCurrentIndex(FModel->index(FModel->rowCount()-1, 0));

  } else if (this->currentIndex().isValid()) {
    // move currently selected spot to new position
    FModel->setSpotPos(this->currentIndex().row(), APos.x(), APos.y());
  }

  UpdatePreview();
}

//==============================================================================

void ptImageSpotListView::currentChanged(const QModelIndex &current, const QModelIndex &previous) {
  QListView::currentChanged(current, previous);
  emit rowChanged(current);
}

//==============================================================================

