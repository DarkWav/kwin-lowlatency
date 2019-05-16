/********************************************************************
 KWin - the KDE window manager
 This file is part of the KDE project.

Copyright 2019 Roman Gilg <subdiff@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*********************************************************************/
#ifndef KWIN_WAYLAND_OUTPUT_H
#define KWIN_WAYLAND_OUTPUT_H

#include "abstract_output.h"

#include <KWayland/Client/xdgshell.h>

#include <QObject>

namespace KWayland
{
namespace Client
{
class Surface;

class Shell;
class ShellSurface;

class Pointer;
class LockedPointer;
}
}

namespace KWin
{
namespace Wayland
{
class WaylandBackend;

class WaylandOutput : public AbstractOutput
{
    Q_OBJECT
public:
    explicit WaylandOutput(KWayland::Client::Surface *surface, QObject *parent = nullptr);
    ~WaylandOutput();

    virtual void lockPointer(KWayland::Client::Pointer *pointer, bool lock) {
        Q_UNUSED(pointer)
        Q_UNUSED(lock)
    }

    virtual bool pointerIsLocked() { return false; }

    QSize pixelSize() const override;

    /**
     * @brief defines the geometry of the output
     * @param logicalPosition top left position of the output in compositor space
     * @param pixelSize output size as seen from the outside
     */
    void setGeometry(const QPoint &logicalPosition, const QSize &pixelSize);

    KWayland::Client::Surface* surface() const {
        return m_surface;
    }

    bool rendered() const {
        return m_rendered;
    }
    void resetRendered() {
        m_rendered = false;
    }

Q_SIGNALS:
    void sizeChanged(const QSize &size);
    void frameRendered();

private:
    KWayland::Client::Surface *m_surface;

    QSize m_pixelSize;
    bool m_rendered = false;
};

class ShellOutput : public WaylandOutput
{
public:
    ShellOutput(KWayland::Client::Surface *surface,
                KWayland::Client::Shell *shell,
                WaylandBackend *backend);
    ~ShellOutput();

private:
    KWayland::Client::ShellSurface *m_shellSurface = nullptr;
};

class XdgShellOutput : public WaylandOutput
{
public:
    XdgShellOutput(KWayland::Client::Surface *surface,
                   KWayland::Client::XdgShell *xdgShell,
                   WaylandBackend *backend, int number);
    ~XdgShellOutput();

    void lockPointer(KWayland::Client::Pointer *pointer, bool lock) override;

private:
    void handleConfigure(const QSize &size, KWayland::Client::XdgShellSurface::States states, quint32 serial);
    void updateWindowTitle();

    KWayland::Client::XdgShellSurface *m_xdgShellSurface = nullptr;
    WaylandBackend *m_backend;
    int m_number;
    KWayland::Client::LockedPointer *m_pointerLock = nullptr;
    bool m_hasPointerLock = false;
};

}
}

#endif
