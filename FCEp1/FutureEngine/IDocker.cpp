
#include "IDocker.h"
#include "IWindow.h"
#include "UIHelp.h"
#include "FutureApp.h"
#include "GameUI.h"
#include "Texture2D.h"

IDocker::IDocker(glm::vec2 position, glm::vec2 size) : IControl(position, size)
{
    InitDockZones();
    m_HoveringWindow = nullptr;
    m_HoveringArea = DOCK_NONE;
    m_HighlightAlpha = 0.0f;
    m_Text = "Docking Area";
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            Filled[x][y] = false;
        }
    }
    //Filled[0][0] =true;
   // Filled[0][2] =true;
    Size[0] = 0.25f;// glm::vec2(0.25f, 0.25f);
    Size[1] = 0.5f; //glm::vec2(0.25f, 0.5f);
    Size[2] = 0.25f;// glm::vec2(0.25f, 0.25f);
    SetDockType(m_Fill);

    m_OverDockImage = new Texture2D("engine/ui/overdock.png");
    CreateZones();
}


void IDocker::AfterSet() {

    m_HoveringWindow = nullptr;
    m_HoveringArea = DOCK_NONE;
    m_HighlightAlpha = 0.0f;
    m_Text = "Docking Area";
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            Filled[x][y] = false;
        }
    }
    //Filled[0][0] =true;
   // Filled[0][2] =true;
    Size[0] = 0.25f;// glm::vec2(0.25f, 0.25f);
    Size[1] = 0.5f; //glm::vec2(0.25f, 0.5f);
    Size[2] = 0.25f;// glm::vec2(0.25f, 0.25f);
    //SetDockType(m_Fill);
    //
    // 
    // CreateZones();
   // Rebuild();

    for (DockZone& area : m_DockZones) {

        switch (area.area) {
        case DockArea::DOCK_LEFT:

            area.position = glm::vec2(0, m_Size.y * 0.25);
            area.size = glm::vec2(m_Size.x * 0.25f, m_Size.y * 0.5);

                break;
        case DockArea::DOCK_RIGHT:

            area.position = glm::vec2(m_Size.x - (m_Size.x * 0.25), m_Size.y * 0.25);
            area.size = glm::vec2(m_Size.x * 0.25f, m_Size.y * 0.5);


            break;
        case DockArea::DOCK_TOP:

            area.position = glm::vec2(m_Size.x * 0.25, 0);
            area.size = glm::vec2(m_Size.x * 0.5f, m_Size.y * 0.25f);

            break;

        case DockArea::DOCK_BOTTOM:

            area.position = glm::vec2(m_Size.x * 0.25, m_Size.y - (m_Size.y * 0.25f));
            area.size = glm::vec2(m_Size.x * 0.5f, m_Size.y * 0.25f);

            break;
        case DockArea::DOCK_CENTER:

            area.position = glm::vec2(m_Size.x * 0.25, m_Size.y * 0.25);
            area.size = glm::vec2(m_Size.x * 0.5f, m_Size.y * 0.5f);

            break;
        }

    }


    Rebuild();

    //m_DockZones.clear();
   // ReCreateZones();
    

}

void ReCreateZones() {

}

void IDocker::CreateZones() {

    auto left = GetLeft();
   auto right = GetRight();
   // 
   // 
    auto bottom = GetBottom();
    auto top = GetTop();
   // 
    auto centre = GetCentre();
}

DockZone IDocker::GetCentre() {

    int startX = -1;
    int startY = -1;
    int endX = -1;
    int endY = -1;

    for (int y = 0; y < 3; y++)
    {
        for (int x = 0; x < 3; x++) {

            if (!Filled[x][y]) {

                if (startX == -1) {
                    startX = x;
                    startY = y;
                }
                else {
                    endX = x;
                    endY = y;
                }

            }

        }
    }

    int px = startX * (m_Size.x * 0.3333);
    int py = startY * (m_Size.y * 0.3333);

    DockZone area;
    area.area = DockArea::DOCK_CENTER;
    area.startX = startX;
    area.endX = endX;
    area.startY = startY;
    area.endY = endY;

    area.position = glm::vec2(m_Size.x * 0.25, m_Size.y * 0.25);
    area.size = glm::vec2(m_Size.x * 0.5f, m_Size.y * 0.5f);
    area.Valid = true;
    m_DockZones.push_back(area);
    return area;


    int a = 5;

    return DockZone();
}

DockZone IDocker::GetBottom() {

    int startX = -1;
    int endX = -1;

    for (int x = 0; x< 3; x++) {
        if (!Filled[x][2])
        {
            if (startX == -1)
            {
                startX = x;
                endX = x;
            }
            else {
                endX = x;
            }
        }
        else {

            if (startX != -1) {
                break;
            }
            //if (startX != -1)
           // {

            //}
        }
    }


    //DockZone area;
    //area.area = DockArea::DOCK_BOTTOM;



    DockZone area;
    area.area = DockArea::DOCK_BOTTOM;
    area.startX = startX;
    area.endX = endX;
    area.startY = 2;
    area.endY = 2;
  
    area.position = glm::vec2(m_Size.x*0.25, m_Size.y - (m_Size.y*0.25f));
    area.size = glm::vec2(m_Size.x *0.5f ,m_Size.y*0.25f);
    area.Valid = true;
    m_DockZones.push_back(area);


    return area;

    if (startX != -1) {
        area.startX = startX;
        area.endX = endX;


        float dx = startX / 2.0f;
        float sx = 0;

        if (startX == 0) {
            dx = 0;
            if (endX == 0)
            {
                sx = m_Size.x * 0.3333;
            }
            if (endX == 1) {
                sx = m_Size.x * 0.6666;
            }
            if (endX == 2) {
                sx = m_Size.x;
            }
        }
        if (startX == 1) {
            dx = m_Size.x * 0.3333;
        }

        int stx = startX * (m_Size.x * 0.3333);
        int di = (endX - startX + 1);

        //area.position = glm::vec2(0, startY);

        //int dif = endY - startY;
        area.position = glm::vec2(stx,m_Size.y * 0.66666);
        area.size = glm::vec2(di * (m_Size.x * 0.3333),m_Size.y*0.3333);

    }
    else {
        area.occupied = false;
    }
    //area.
    m_DockZones.push_back(area);
    return area;

    //bool b1 = Filled[0][0];
    //bool b2 = Filled[0][1];
    //bool b3 = Filled[0][2];




}


DockZone IDocker::GetTop() {

    int startX = -1;
    int endX = -1;

    for (int x = 0; x < 3; x++) {
        if (!Filled[x][0])
        {
            if (startX == -1)
            {
                startX = x;
                endX = x;
            }
            else {
                endX = x;
            }
        }
        else {

            if (startX != -1) {

                break;
            }
            //if (startX != -1)
           // {

            //}
        }
    }

    DockZone area;
    area.area = DockArea::DOCK_TOP;
    area.startX = startX;
    area.endX = endX;
    area.startY = 0;
    area.endY = 0;

    area.position = glm::vec2(m_Size.x * 0.25,0);
    area.size = glm::vec2(m_Size.x * 0.5f, m_Size.y * 0.25f);
    area.Valid = true;
    m_DockZones.push_back(area);
    return area;

    if (startX != -1) {
        area.startX = startX;
        area.endX = endX;


        float dx = startX / 2.0f;
        float sx = 0;

        if (startX == 0) {
            dx = 0;
            if (endX == 0)
            {
                sx = m_Size.x * 0.3333;
            }
            if (endX == 1) {
                sx = m_Size.x * 0.6666;
            }
            if (endX == 2) {
                sx = m_Size.x;
            }
        }
        if (startX == 1) {
            dx = m_Size.x * 0.3333;
        }

        int stx = startX * (m_Size.x * 0.3333);
        int di = (endX - startX + 1);

        //area.position = glm::vec2(0, startY);

        //int dif = endY - startY;
        area.position = glm::vec2(stx,0);
        area.size = glm::vec2(di * (m_Size.x * 0.3333), m_Size.y * 0.3333);

    }
    else {
        area.occupied = false;
    }
    //area.
    m_DockZones.push_back(area);
    return area;

    //bool b1 = Filled[0][0];
    //bool b2 = Filled[0][1];
    //bool b3 = Filled[0][2];




}

DockZone IDocker::GetRight() {


    int startY = -1;
    int endY = -1;

    for (int y = 0; y < 3; y++) {
        if (!Filled[2][y])
        {
            if (startY == -1)
            {
                startY = y;
                endY = y;
            }
            else {
                endY = y;
            }
        }
        else {

            if (startY != -1) {
                break;
            }
            //if (startX != -1)
           // {

            //}
        }
    }



    DockZone area;
    area.area = DockArea::DOCK_RIGHT;
    area.startX = 0;
    area.startY = startY;
    area.endY = endY;
    area.position = glm::vec2(m_Size.x-(m_Size.x*0.25), m_Size.y * 0.25);
    area.size = glm::vec2(m_Size.x * 0.25f, m_Size.y * 0.5);

    area.Valid = true;
 
        m_DockZones.push_back(area);

    return area;

    if (startY != -1) {
        area.startY = startY;
        area.endY = endY;


        float dy = startY / 2.0f;
        float sy = 0;

        if (startY == 0) {
            dy = 0;
            if (endY == 0)
            {
                sy = m_Size.y * 0.3333;
            }
            if (endY == 1) {
                sy = m_Size.y * 0.6666;
            }
            if (endY == 2) {
                sy = m_Size.y;
            }
        }
        if (startY == 1) {
            dy = m_Size.y * 0.3333;
        }

        int sty = startY * (m_Size.y * 0.3333);
        int di = (endY - startY + 1);

        //area.position = glm::vec2(0, startY);

        //int dif = endY - startY;
        area.position = glm::vec2(m_Size.x*0.66666, sty);
        area.size = glm::vec2(m_Size.x * 0.3333, di * (m_Size.y * 0.3333));

    }
    else {
        area.occupied = false;
    }
    //area.
    m_DockZones.push_back(area);
    return area;

    //bool b1 = Filled[0][0];
    //bool b2 = Filled[0][1];
    //bool b3 = Filled[0][2];




}


DockZone IDocker::GetDockArea(DockArea area)
{

    for (auto check : m_DockZones)
    {
        if (check.area == area) {
            return check;
        }
    }

    return DockZone();

}

DockZone IDocker::GetLeft() {


    //auto dleft = GetDockArea(DOCK_LEFT);

    int startY = -1;
    int endY = -1;

    for (int y = 0; y < 3; y++) {
        if (!Filled[0][y])
        {
            if (startY == -1)
            {
                startY = y;
                endY = y;
            }
            else {
                endY = y;
            }
        }
        else {

            if (startY != -1) {
                break;
            }
            //if (startX != -1)
           // {

            //}
        }
    }

    

    auto left = GetDockArea(DockArea::DOCK_LEFT);



    DockZone area;
    area.area = DockArea::DOCK_LEFT;
    area.startX = 0;
    area.startY = startY;
    area.endY = endY;
    area.position = glm::vec2(0, m_Size.y * 0.25);
    area.size = glm::vec2(m_Size.x * 0.25f, m_Size.y * 0.5);

    area.Valid = true;
    m_DockZones.push_back(area);

    /*

    if (startY != -1) {
        area.startY = startY;
        area.endY = endY;


        float dy = startY / 2.0f;
        float sy = 0;

        if (startY == 0) {
            dy = 0;
            if (endY == 0)
            {
                sy = m_Size.y * 0.3333;
            }
            if (endY == 1) {
                sy = m_Size.y * 0.6666;
            }
            if (endY == 2) {
                sy = m_Size.y;
            }
        }
        if (startY == 1) {
            dy = m_Size.y * 0.3333;
        }

        int sty = startY * (m_Size.y * 0.33);
        int di = (endY - startY + 1);

        //area.position = glm::vec2(0, startY);
        
        //int dif = endY - startY;
        area.position = glm::vec2(0, sty);
        area.size = glm::vec2(m_Size.x*0.3333, di*(m_Size.y*0.3333));

    }
    else {
        area.occupied = false;
    }
    //area.
    m_DockZones.push_back(area);
    return area;

    //bool b1 = Filled[0][0];
    //bool b2 = Filled[0][1];
    //bool b3 = Filled[0][2];
    */
    return area;
}

void IDocker::InitDockZones()
{
    // Clear any existing zones
 
}



void IDocker::WindowCancel()
{
  
    // Reset hovering state
    m_HoveringWindow = nullptr;
    m_HoveringArea = DOCK_NONE;

    // Reset animation state
    m_HighlightAlpha = 0.0f;
    m_DockingStarted = false;
}

void IDocker::WindowOver(IWindow* window, glm::vec2 position)
{
    // Reset tracking if not dragging or no window
    int a = 5;
    auto overZone = GetZoneAt(position);
    if (overZone.Valid) {
        m_OverZone = overZone;
    }
    else {
        m_OverZone.Valid = false;
    }
    m_DockingStarted = true;
}

DockZone IDocker::GetZoneAt(glm::vec2 position) {

    auto rp = GetRenderPosition();


    for (auto area : m_DockZones) {


        if (position.x > rp.x+area.position.x && position.y > rp.y+area.position.y && position.x < (rp.x+area.position.x + area.size.x) && position.y < (rp.y+area.position.y + area.size.y)) {
            return area;
        }

    }
    return DockZone();
}


void IDocker::Update(float delta)
{
    // Update highlight animation
    if (m_HoveringArea != DOCK_NONE) {
        // Pulse the highlight effect smoothly
        m_HighlightAlpha += delta * 2.0f; // Control the speed of pulsing

        // Keep the value cycling for the sine wave
        if (m_HighlightAlpha > 1.0f) {
            m_HighlightAlpha = fmod(m_HighlightAlpha, 1.0f);
        }
    }
    else {
        // Fade out the highlight if no window is hovering
        m_HighlightAlpha = std::max(0.0f, m_HighlightAlpha - delta * 3.0f);
    }
    WindowCancel();
    // Update all children (docked windows)
    UpdateChildren(delta);
}

void IDocker::Render()
{
    // Render the docker background
    auto pos = GetRenderPosition();
    //UIHelp::DrawRect(pos, m_Size, glm::vec4(1.15f, 0.15f, 0.15f, 0.5f));

    for (auto area : m_DockZones) {

        glm::vec4 col;

        switch (area.area) {
        case DOCK_LEFT:
            col = glm::vec4(1, 0, 0, 0.35);
            break;
        case DOCK_RIGHT:
            col = glm::vec4(0, 1, 0, 0.35);
            break;
        case DOCK_BOTTOM:
            col = glm::vec4(0, 0, 1, 0.35);
            break;
        case DOCK_TOP:
            col = glm::vec4(1, 1, 0, 0.35f);
            break;
        case DOCK_CENTER:

            col = glm::vec4(1, 0, 1, 0.35f);

            break;
        default:
            col = glm::vec4(0, 0, 0, 0);
            break;
        }


        if (m_OverZone.Valid) {
            if (area.area == m_OverZone.area) {

                ///col = col * 2.0f;
                col = glm::vec4(1, 1, 1, 1);

                auto dim = GetAreaDimensions(area.area,false);

                dim.x += GetRenderPosition().x;
                dim.y += GetRenderPosition().y;

                UIHelp::DrawImage(glm::vec2(dim.x,dim.y),glm::vec2(dim.z,dim.w), m_OverDockImage, glm::vec4(col));

            }
        }

        if (m_DockingStarted) {
            UIHelp::DrawImage(GetRenderPosition() + area.position, area.size, m_OverDockImage, glm::vec4(0, 1, 0, 1));
        }
    }
    // Ensure zones are updated and accurate before rendering
    // This ensures the visualized zones match the actual dock areas
  
    RenderChildren();
}


void IDocker::DockWindow(IWindow* window, DockArea darea) {


    //window->SetIsDocked();
    window->SetIsDocked(true);

    for (auto zone : m_DockZones) {

        if (zone.area == darea) {

            zone.Window = window;

            if (zone.area != DOCK_NONE) {

                m_DockOrder.push_back(zone);
                window->SetDock(this);
                window->SetDocked(true);
                if (window->GetRoot() != nullptr) {
                    window->GetRoot()->RemoveChild(window);
                }
                AddChild(window);

                window->Set(window->GetPosition() - this->GetRenderPosition(), window->GetSize());
                //    window->set
                return;


            }

        }

    }


}

void IDocker::DockWindow(IWindow* window, glm::vec2 position) {


    if (GetRoot()->GetRoot() == window) {
        return;
   }


    //window->SetIsDocked();

    auto zone = GetZoneAt(position);

    if (zone.area == DOCK_NONE) return;

    if (zone.Window != nullptr)
    {
        WindowCancel();
        return;
    }

    zone.Window = window;
    window->SetIsDocked(true);

    if (zone.area != DOCK_NONE) {

        m_DockOrder.push_back(zone);
        window->SetDock(this);
        window->SetDocked(true);
        window->GetRoot()->RemoveChild(window);
        AddChild(window);
        window->Set(window->GetPosition() - this->GetRenderPosition(), window->GetSize());
    //    window->set



    }

    Rebuild();

    int b = 5;


}

void IDocker::Rebuild() {

    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            Filled[x][y] = false;
        }
    }

    for (auto dock : m_DockOrder) {


        glm::vec4 dim;

        switch (dock.area) {
        case DOCK_LEFT:

            dock.position = glm::vec2(0, m_Size.y * 0.25);
            dock.size = glm::vec2(m_Size.x * 0.25f, m_Size.y * 0.5);

           dim = GetAreaDimensions(DockArea::DOCK_LEFT,true);

           break;

        case DOCK_RIGHT:

            dim = GetAreaDimensions(DockArea::DOCK_RIGHT, true);
       

            break;
        case DOCK_BOTTOM:

            dim = GetAreaDimensions(DockArea::DOCK_BOTTOM,true);


            break;
        case DOCK_TOP:

            dim = GetAreaDimensions(DockArea::DOCK_TOP, true);

            break;
        case DOCK_CENTER:

            int startX = dock.startX;
            int startY = dock.startY;
            int endX = dock.endX;
            int endY = dock.endY;

            startX = -1;
            startY = -1;

            for (int y = 0; y < 3; y++)
            {
                for (int x = 0; x < 3; x++) {

                    if (!Filled[x][y]) {

                        if (startX == -1) {
                            startX = x;
                            startY = y;
                        }
                        else {
                            endX = x;
                            endY = y;
                        }

                    }

                }
            }


            auto ls = GetLeftSize(startY, endY);
            auto ts = GetBottomSize(startX, endX);


            dim.x = ts.x;
            dim.y = ls.x;
            dim.z = ts.y;
            dim.w = ls.y;

            int b = 5;

            //dim = GetAreaDimensions(DockArea::DOCK_CENTER, true);

            break;
        }

        //dim.x = GetRenderPosition().x - dim.x;
        //dim.y = GetRenderPosition().y - dim.y;

        glm::vec2 dpos = glm::vec2(dim.x, dim.y);
        dpos = dpos - GetRenderPosition();

        //dock.Window->Set(dpos, dock.Window->GetSize(), false);
        

        dim.x += 3;
        dim.y += 3;
        dim.z -= 6;
        dim.w -= 6;
        dock.Window->Set(glm::vec2(dim.x, dim.y), glm::vec2(dim.z, dim.w),true);


    }

}

glm::vec4 IDocker::GetAreaDimensions(DockArea area,bool fill) {


    if (area == DOCK_CENTER) {


        


    }else 
    if (area == DOCK_LEFT) {

        int startY = -1;
        int endY = -1;

        for (int y = 0; y < 3; y++) {
            if (!Filled[0][y])
            {
                if (startY == -1)
                {
                    startY = y;
                    endY = y;
                }
                else {
                    endY = y;
                }
            }
            else {

                if (startY != -1) {
                    break;
                }
                //if (startX != -1)
               // {

                //}
            }
        }


        auto ls = GetLeftSize(startY, endY);


       int b = -5;

        float width = m_Size.x * 0.3333;
        float y = startY * m_Size.y * 0.3333f;
        float height = (1 + endY) * m_Size.y * 0.3333f;

        if (fill) {

            for (int y = startY; y <= endY; y++) {
                Filled[0][y] = true;
            }

        }
       
        return glm::vec4(0, ls.x, m_Size.x*0.25f, ls.y);

     //   int a = 5;

    }
    else if (area == DOCK_RIGHT) {

        int startY = -1;
        int endY = -1;

        for (int y = 0; y < 3; y++) {
            if (!Filled[2][y])
            {
                if (startY == -1)
                {
                    startY = y;
                    endY = y;
                }
                else {
                    endY = y;
                }
            }
            else {

                if (startY != -1) {
                    break;
                }
                //if (startX != -1)
               // {

                //}
            }
        }


        auto ls = GetLeftSize(startY, endY);


        int b = -5;

        float width = m_Size.x * 0.3333;
        float y = startY * m_Size.y * 0.3333f;
        float height = (1 + endY) * m_Size.y * 0.3333f;

        if (fill) {

            for (int y = startY; y <= endY; y++) {
                Filled[2][y] = true;
            }

        }

        return glm::vec4(m_Size.x-(m_Size.x*0.25), ls.x, m_Size.x * 0.25f, ls.y);


    }
    else if (area == DOCK_BOTTOM) {

        int startX = -1;
        int endX = -1;

        for (int x = 0; x < 3; x++) {
            if (!Filled[x][2])
            {
                if (startX == -1)
                {
                    startX = x;
                    endX = x;
                }
                else {
                    endX = x;
                }
            }
            else {

                if (startX != -1) {
                    break;
                }
                //if (startX != -1)
               // {

                //}
            }
        }
        
        float height = m_Size.y * 0.255;
        float x = startX * m_Size.x * 0.3333f;
        float width = (1 + endX) * m_Size.x * 0.3333f;
        float dif = endX - startX;

        float dw = (m_Size.x * 0.333) + ((dif) * (m_Size.x * 0.3333));



        // - 0,1,2 = 2
        //  [0],1,2 = 1

        if (fill) {
            for (int x = startX; x <= endX; x++) {
                Filled[x][2] = true;
            }
        }

        auto ls = GetBottomSize(startX, endX);

        return glm::vec4(ls.x,m_Size.y-(m_Size.y*0.25), ls.y, m_Size.y * 0.25f);

        return glm::vec4(x+1, m_Size.y-(m_Size.y*0.25f)+1, dw-2, height-2);

    }
    else if(area==DOCK_TOP) {

        int startX = -1;
        int endX = -1;

        for (int x = 0; x < 3; x++) {
            if (!Filled[x][0])
            {
                if (startX == -1)
                {
                    startX = x;
                    endX = x;
                }
                else {
                    endX = x;
                }
            }
            else {

                if (startX != -1) {

                    break;
                }
                //if (startX != -1)
               // {

                //}
            }
        }
        if (fill) {
            for (int x = startX; x <= endX; x++) {
                Filled[x][0] = true;
            }
        }

        auto ls = GetBottomSize(startX, endX);

        return glm::vec4(ls.x,0, ls.y, m_Size.y * 0.25f);

    }

    return glm::vec4(0, 0, 0, 0);

}


glm::vec2 IDocker::GetLeftSize(int sy, int ey) {

    float height = 0;

    for (int y = sy; y <= ey; y++) {
        height += Size[y] * m_Size.y;// FutureApp::m_Inst->GetHeight();
    }

    int start = 0;
    for (int y = 0; y < sy; y++) {

        start += Size[y] * m_Size.y;// FutureApp::m_Inst->GetHeight();
    }

    return glm::vec2(start, height);

}

glm::vec2 IDocker::GetBottomSize(int sx, int ex) {

    float width = 0;

    for (int x = sx; x <= ex; x++) {
        width += Size[x] * m_Size.x;// FutureApp::m_Inst->GetWidth();
    }

    int start = 0;
    for (int x = 0; x < sx; x++) {

        start += Size[x] * m_Size.x; FutureApp::m_Inst->GetWidth();
    }

    return glm::vec2(start, width);

}


void IDocker::UndockWindow(IWindow* window) {

    int idx = 0;

    for (auto dock : m_DockOrder) {



        if (dock.Window == window) {


            // Remove element at index 2 (value 30)
            m_DockOrder.erase(m_DockOrder.begin() + idx);
            RemoveChild(dock.Window);
            GameUI::m_Inst->GetRoot()->AddChild(dock.Window);
            dock.Window->Set(glm::vec2(GetRenderPosition().x, GetRenderPosition().y) + dock.Window->GetPosition());
            break;


            int b = 5;

        }
        idx++;

    }
    int a = 5;
    Rebuild();

}

bool IDocker::HasNot(IWindow* window) {

    for (auto dock : m_DockOrder) {

        if (dock.Window == window) return false;

    }

    return true;

}