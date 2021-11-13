// Version: 1.1.1

import QtQuick 2.12

Item
{
    id: pulltorefreshhandler
    anchors.fill: parent

    property var flickable: parent
    property int threshold: 20
    readonly property alias is_pulldown: private_props.m_is_pulldown
    readonly property alias is_pullup: private_props.m_is_pullup
    readonly property alias is_pulling_down: private_props.m_is_pulling_down
    readonly property alias is_pulling_up: private_props.m_is_pulling_up
    readonly property alias progress: private_props.m_progress
    property alias swipe_up_hint: up_hint_loader
    property alias swipe_down_hint: down_hint_loader
    property alias refresh_indicator: refresh_indicator_loader
    property int indicator_drag_direction: PullToRefreshHandler.TOPTOBOTTOM

    enum IndicatorDragDirection
    {
        TOPTOBOTTOM,
        BOTTOMTOTOP
    }

    // Delegates properties:
    property Component swipe_up_hint_delegate: null
    property Component swipe_down_hint_delegate: null
    property Component refresh_indicator_delegate: RefreshIndicator {}

    QtObject
    {
        id: private_props
        property bool m_is_pulldown: false
        property bool m_is_pullup: false
        property bool m_is_pulling_down: false
        property bool m_is_pulling_up: false
        property int  m_threshold: (threshold * flickable.height) / 100
        property real m_progress: {
            if (!pulltorefreshhandler.enabled)
                return 0;
            if (!flickable || !m_threshold)
                return 0;
            return (indicator_drag_direction === PullToRefreshHandler.TOPTOBOTTOM) ?
                        (flickable.verticalOvershoot * -100) / m_threshold : (flickable.verticalOvershoot * 100) / m_threshold;
        }
    }

    signal pulldown()
    signal pullup()
    signal pulldownrelease()
    signal pulluprelease()

    Connections
    {
        target: flickable
        enabled: pulltorefreshhandler.enabled
        function onVerticalOvershootChanged()
        {
            if (!flickable.verticalOvershoot)
            {
                private_props.m_is_pulling_down = false;
                private_props.m_is_pulling_up = false;
                if (private_props.m_is_pulldown)
                {
                    private_props.m_is_pulldown = false;
                    pulltorefreshhandler.pulldownrelease();
                }
                if (private_props.m_is_pullup)
                {
                    private_props.m_is_pullup = false;
                    pulltorefreshhandler.pulluprelease();
                }
                return;
            }

            if (flickable.verticalOvershoot < 0)
            {
                private_props.m_is_pulling_down = true;
                if (Math.abs(flickable.verticalOvershoot) > private_props.m_threshold)
                {
                    private_props.m_is_pulldown = true;
                    pulltorefreshhandler.pulldown();
                }
            }
            else if (flickable.verticalOvershoot > 0)
            {
                private_props.m_is_pulling_up = true;
                if (flickable.verticalOvershoot > private_props.m_threshold)
                {
                    private_props.m_is_pullup = true;
                    pulltorefreshhandler.pullup();
                }
            }
        }
    }

    Loader
    {
        id: up_hint_loader
        sourceComponent: (
                             pulltorefreshhandler.enabled &&
                             swipe_up_hint_delegate &&
                             flickable.contentHeight &&
                             flickable.atYEnd &&
                             !is_pullup
                             ) ? swipe_up_hint_delegate : undefined
        anchors.bottom: parent.bottom
    }

    Loader
    {
        id: down_hint_loader
        sourceComponent: (
                             pulltorefreshhandler.enabled &&
                             swipe_down_hint_delegate &&
                             flickable.contentHeight &&
                             flickable.atYBeginning &&
                             !is_pulldown
                             ) ? swipe_down_hint_delegate : undefined
        anchors.bottom: parent.top
    }

    Loader
    {
        id: refresh_indicator_loader
        property real drag_progress: private_props.m_progress
        sourceComponent: (
                             pulltorefreshhandler.enabled &&
                             refresh_indicator.active &&
                             (private_props.m_progress > 0)
                             ) ?
                             pulltorefreshhandler.refresh_indicator_delegate : undefined
    }
}
