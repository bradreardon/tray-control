#pragma once

#include <nlohmann/json.hpp>
#include "StatusNotifierItem.h"

using json = nlohmann::json;

/*
    Simple helper for nulling a key in output JSON if there's an error when fetching it.
*/
template <typename T, typename E>
inline void json_set_or_null(json& j, std::string_view key, const std::expected<T, E>& value)
{
    j[key] = value ? json(*value) : json(nullptr);
}

/*
    nlohmann/json finds `to_json` methods using ADL, so we define some helper methods
    here to enable automatic JSON serialization of a few types.
*/
inline void to_json(json& j, const StatusNotifierItem::ToolTip& t)
{
    j = nlohmann::json{
        {"icon_name", t.iconName},
        {"title",     t.title},
        {"description", t.description},
    };
}

inline void to_json(json& j, StatusNotifierItem& t)
{
    j = json::object();

    json_set_or_null(j, "category", t.getCategory());
    json_set_or_null(j, "id", t.getId());
    json_set_or_null(j, "title", t.getTitle());
    json_set_or_null(j, "status", t.getStatus());
    json_set_or_null(j, "windowId", t.getWindowId());
    json_set_or_null(j, "iconName", t.getIconName());
    json_set_or_null(j, "overlayIconName", t.getOverlayIconName());
    json_set_or_null(j, "attentionIconName", t.getAttentionIconName());
    json_set_or_null(j, "attentionMovieName", t.getAttentionMovieName());

    json jProcess = json::object();
    json_set_or_null(jProcess, "pid", t.getPID());

    j["process"] = jProcess;

    // TODO: re-add after getToolTip is implemented
    // json_set_or_null(j, "toolTip", t.getToolTip());
}
