/***************************************************************************
 *   Copyright (c) 2004 Juergen Riegel <juergen.riegel@web.de>             *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#include "PreCompiled.h"

#ifndef _PreComp_
# include <QAction>
# include <QMenu>
#endif

#include <Base/Console.h>
#include <Base/Exception.h>
#include <App/Document.h>
#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/Control.h>

#include <Mod/Part/App/PartFeature.h>
#include <Mod/Part/App/PrimitiveFeature.h>

#include "ViewProvider.h"
#include "DlgPrimitives.h"


using namespace PartGui;

PROPERTY_SOURCE(PartGui::ViewProviderPart, PartGui::ViewProviderPartExt)


ViewProviderPart::ViewProviderPart()
{
}

ViewProviderPart::~ViewProviderPart()
{
}

bool ViewProviderPart::doubleClicked(void)
{
    std::string Msg("Edit ");
    Msg += this->pcObject->Label.getValue();
    try {
        Gui::Command::openCommand(Msg.c_str());
        FCMD_SET_EDIT(pcObject);
        return true;
    }
    catch (const Base::Exception& e) {
        Base::Console().Error("%s\n", e.what());
        return false;
    }
}

void ViewProviderPart::applyColor(const Part::ShapeHistory& hist,
                                  const std::vector<App::Color>& colBase,
                                  std::vector<App::Color>& colBool)
{
    std::map<int, std::vector<int> >::const_iterator jt;
    // apply color from modified faces
    for (jt = hist.shapeMap.begin(); jt != hist.shapeMap.end(); ++jt) {
        std::vector<int>::const_iterator kt;
        for (kt = jt->second.begin(); kt != jt->second.end(); ++kt) {
            colBool[*kt] = colBase[jt->first];
        }
    }
}

void ViewProviderPart::applyTransparency(const float& transparency,
                                  std::vector<App::Color>& colors)
{
    if (transparency != 0.0) {
        // transparency has been set object-wide
        std::vector<App::Color>::iterator j;
        for (j = colors.begin(); j != colors.end(); ++j) {
            // transparency hasn't been set for this face
            if (j->a == 0.0)
                j->a = transparency/100.0; // transparency comes in percent
        }
    }
}

// ----------------------------------------------------------------------------

PROPERTY_SOURCE(PartGui::ViewProviderPrimitive, PartGui::ViewProviderPart)

ViewProviderPrimitive::ViewProviderPrimitive()
{
}

ViewProviderPrimitive::~ViewProviderPrimitive()
{

}

void ViewProviderPrimitive::setupContextMenu(QMenu* menu, QObject* receiver, const char* member)
{
    QAction* act;
    act = menu->addAction(QObject::tr("Edit %1").arg(QString::fromUtf8(getObject()->Label.getValue())), receiver, member);
    act->setData(QVariant((int)ViewProvider::Default));
    ViewProviderPart::setupContextMenu(menu, receiver, member);
}

bool ViewProviderPrimitive::setEdit(int ModNum)
{
    if (ModNum == ViewProvider::Default) {
        if (Gui::Control().activeDialog())
            return false;
        PartGui::TaskPrimitivesEdit* dlg
            = new PartGui::TaskPrimitivesEdit(dynamic_cast<Part::Primitive*>(getObject()));
        Gui::Control().showDialog(dlg);
        return true;
    }
    else {
        ViewProviderPart::setEdit(ModNum);
        return true;
    }
}

void ViewProviderPrimitive::unsetEdit(int ModNum)
{
    if (ModNum == ViewProvider::Default) {
        Gui::Control().closeDialog();
    }
    else {
        ViewProviderPart::unsetEdit(ModNum);
    }
}

// ----------------------------------------------------------------------------

void ViewProviderShapeBuilder::buildNodes(const App::Property* , std::vector<SoNode*>& ) const
{
}

void ViewProviderShapeBuilder::createShape(const App::Property* , SoSeparator* ) const
{
}
