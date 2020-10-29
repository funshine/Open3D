// ----------------------------------------------------------------------------
// -                        Open3D: www.open3d.org                            -
// ----------------------------------------------------------------------------
// The MIT License (MIT)
//
// Copyright (c) 2020 www.open3d.org
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------------------------------------------------------

#include "open3d/visualization/visualizer/Receiver.h"

#include <zmq.hpp>

#include "open3d/geometry/PointCloud.h"
#include "open3d/geometry/TriangleMesh.h"
#include "open3d/io/rpc/MessageUtils.h"
#include "open3d/io/rpc/Messages.h"
#include "open3d/visualization/gui/Application.h"
#include "open3d/visualization/rendering/Material.h"
#include "open3d/visualization/visualizer/GuiVisualizer.h"

using namespace open3d::io::rpc;
using namespace open3d::utility;

namespace open3d {
namespace visualization {

std::shared_ptr<zmq::message_t> Receiver::ProcessMessage(
        const messages::Request& req,
        const messages::SetMeshData& msg,
        const MsgpackObject& obj) {
    if (!scene_) {
        LogError("scene is null");
    }

    std::string errstr(":");
    if (!msg.data.CheckMessage(errstr)) {
        auto status_err = messages::Status::ErrorProcessingMessage();
        status_err.str += errstr;
        msgpack::sbuffer sbuf;
        messages::Reply reply{status_err.MsgId()};
        msgpack::pack(sbuf, reply);
        msgpack::pack(sbuf, status_err);
        return std::shared_ptr<zmq::message_t>(
                new zmq::message_t(sbuf.data(), sbuf.size()));
    }

    if (msg.data.faces.CheckNonEmpty()) {
        // create a TriangleMesh
        auto mesh = std::make_shared<geometry::TriangleMesh>();

        errstr = "";
        if (!msg.data.vertices.CheckType(
                {messages::TypeStr<float>(), messages::TypeStr<double>()},
                errstr)) {
            errstr = "Ignoring vertices. vertices have wrong data type:" +
                     errstr;
            LogInfo(errstr.c_str());
        } else {
            mesh->vertices_.resize(msg.data.vertices.shape[0]);
            if (msg.data.vertices.type == messages::TypeStr<float>()) {
                const float* ptr = msg.data.vertices.Ptr<float>();
                for (int64_t i = 0; i < msg.data.vertices.shape[0]; ++i) {
                    mesh->vertices_[i].x() = ptr[0];
                    mesh->vertices_[i].y() = ptr[1];
                    mesh->vertices_[i].z() = ptr[2];
                    ptr += 3;
                }
            }
            if (msg.data.vertices.type == messages::TypeStr<double>()) {
                const double* ptr = msg.data.vertices.Ptr<double>();
                for (int64_t i = 0; i < msg.data.vertices.shape[0]; ++i) {
                    mesh->vertices_[i].x() = ptr[0];
                    mesh->vertices_[i].y() = ptr[1];
                    mesh->vertices_[i].z() = ptr[2];
                    ptr += 3;
                }
            }
        }

        errstr = "";
        if (msg.data.vertex_attributes.count("normals")) {
            const auto& attr_arr = msg.data.vertex_attributes.at("normals");
            auto& mesh_attr = mesh->vertex_normals_;
            if (!attr_arr.CheckType({messages::TypeStr<float>(),
                                     messages::TypeStr<double>()},
                                    errstr)) {
                errstr = "Ignoring normals. normals have wrong data type:" +
                         errstr;
                LogInfo(errstr.c_str());
            } else if (!attr_arr.CheckShape({-1, 3}, errstr)) {
                errstr = "Ignoring normals. normals have wrong shape:" + errstr;
                LogInfo(errstr.c_str());
            } else {
                mesh_attr.resize(attr_arr.shape[0]);
                if (attr_arr.type == messages::TypeStr<float>()) {
                    const float* ptr = attr_arr.Ptr<float>();
                    for (int64_t i = 0; i < attr_arr.shape[0]; ++i) {
                        mesh_attr[i].x() = ptr[0];
                        mesh_attr[i].y() = ptr[1];
                        mesh_attr[i].z() = ptr[2];
                        ptr += 3;
                    }
                }
                if (attr_arr.type == messages::TypeStr<double>()) {
                    const double* ptr = attr_arr.Ptr<double>();
                    for (int64_t i = 0; i < attr_arr.shape[0]; ++i) {
                        mesh_attr[i].x() = ptr[0];
                        mesh_attr[i].y() = ptr[1];
                        mesh_attr[i].z() = ptr[2];
                        ptr += 3;
                    }
                }
            }
        }

        errstr = "";
        if (msg.data.vertex_attributes.count("colors")) {
            const auto& attr_arr = msg.data.vertex_attributes.at("colors");
            auto& mesh_attr = mesh->vertex_colors_;
            if (!attr_arr.CheckType({messages::TypeStr<float>(),
                                     messages::TypeStr<double>()},
                                    errstr)) {
                errstr = "Ignoring colors. colors have wrong data type:" +
                         errstr;
                LogInfo(errstr.c_str());
            } else if (!attr_arr.CheckShape({-1, 3}, errstr)) {
                errstr = "Ignoring colors. colors have wrong shape:" + errstr;
                LogInfo(errstr.c_str());
            } else {
                mesh_attr.resize(attr_arr.shape[0]);
                if (attr_arr.type == messages::TypeStr<float>()) {
                    const float* ptr = attr_arr.Ptr<float>();
                    for (int64_t i = 0; i < attr_arr.shape[0]; ++i) {
                        mesh_attr[i].x() = ptr[0];
                        mesh_attr[i].y() = ptr[1];
                        mesh_attr[i].z() = ptr[2];
                        ptr += 3;
                    }
                }
                if (attr_arr.type == messages::TypeStr<double>()) {
                    const double* ptr = attr_arr.Ptr<double>();
                    for (int64_t i = 0; i < attr_arr.shape[0]; ++i) {
                        mesh_attr[i].x() = ptr[0];
                        mesh_attr[i].y() = ptr[1];
                        mesh_attr[i].z() = ptr[2];
                        ptr += 3;
                    }
                }
            }
        }

        errstr = "";
        if (!msg.data.faces.CheckShape({-1, 3}, errstr)) {
            errstr = "Ignoring faces. Only triangular faces are supported:" +
                     errstr;
            LogInfo(errstr.c_str());
        } else if (!msg.data.faces.CheckType({messages::TypeStr<int32_t>(),
                                              messages::TypeStr<int64_t>()},
                                             errstr)) {
            errstr = "Ignoring faces. Triangles have wrong data type:" + errstr;
            LogInfo(errstr.c_str());
        } else {
            mesh->triangles_.resize(msg.data.faces.shape[0]);
            if (msg.data.faces.type == messages::TypeStr<int32_t>()) {
                const int32_t* ptr = msg.data.faces.Ptr<int32_t>();
                for (int64_t i = 0; i < msg.data.faces.shape[0]; ++i) {
                    mesh->triangles_[i].x() = ptr[0];
                    mesh->triangles_[i].y() = ptr[1];
                    mesh->triangles_[i].z() = ptr[2];
                    ptr += 3;
                }
            }
            if (msg.data.faces.type == messages::TypeStr<int64_t>()) {
                const int64_t* ptr = msg.data.faces.Ptr<int64_t>();
                for (int64_t i = 0; i < msg.data.faces.shape[0]; ++i) {
                    mesh->triangles_[i].x() = ptr[0];
                    mesh->triangles_[i].y() = ptr[1];
                    mesh->triangles_[i].z() = ptr[2];
                    ptr += 3;
                }
            }
        }

        SetGeometry(mesh, msg.path, msg.time, msg.layer);

    } else {
        // create a PointCloud
        auto pcd = std::make_shared<geometry::PointCloud>();
        if (!msg.data.vertices.CheckType(
                {messages::TypeStr<float>(), messages::TypeStr<double>()},
                errstr)) {
            errstr = "Ignoring vertices. vertices have wrong data type:" +
                     errstr;
            LogInfo(errstr.c_str());
        } else {
            pcd->points_.resize(msg.data.vertices.shape[0]);
            if (msg.data.vertices.type == messages::TypeStr<float>()) {
                const float* ptr = msg.data.vertices.Ptr<float>();
                for (int64_t i = 0; i < msg.data.vertices.shape[0]; ++i) {
                    pcd->points_[i].x() = ptr[0];
                    pcd->points_[i].y() = ptr[1];
                    pcd->points_[i].z() = ptr[2];
                    ptr += 3;
                }
            }
            if (msg.data.vertices.type == messages::TypeStr<double>()) {
                const double* ptr = msg.data.vertices.Ptr<double>();
                for (int64_t i = 0; i < msg.data.vertices.shape[0]; ++i) {
                    pcd->points_[i].x() = ptr[0];
                    pcd->points_[i].y() = ptr[1];
                    pcd->points_[i].z() = ptr[2];
                    ptr += 3;
                }
            }

            errstr = "";
            if (msg.data.vertex_attributes.count("normals")) {
                const auto& attr_arr = msg.data.vertex_attributes.at("normals");
                auto& mesh_attr = pcd->normals_;
                if (!attr_arr.CheckType({messages::TypeStr<float>(),
                                         messages::TypeStr<double>()},
                                        errstr)) {
                    errstr = "Ignoring normals. normals have wrong data type:" +
                             errstr;
                    LogInfo(errstr.c_str());
                } else if (!attr_arr.CheckShape({-1, 3}, errstr)) {
                    errstr = "Ignoring normals. normals have wrong shape:" +
                             errstr;
                    LogInfo(errstr.c_str());
                } else {
                    mesh_attr.resize(attr_arr.shape[0]);
                    if (attr_arr.type == messages::TypeStr<float>()) {
                        const float* ptr = attr_arr.Ptr<float>();
                        for (int64_t i = 0; i < attr_arr.shape[0]; ++i) {
                            mesh_attr[i].x() = ptr[0];
                            mesh_attr[i].y() = ptr[1];
                            mesh_attr[i].z() = ptr[2];
                            ptr += 3;
                        }
                    }
                    if (attr_arr.type == messages::TypeStr<double>()) {
                        const double* ptr = attr_arr.Ptr<double>();
                        for (int64_t i = 0; i < attr_arr.shape[0]; ++i) {
                            mesh_attr[i].x() = ptr[0];
                            mesh_attr[i].y() = ptr[1];
                            mesh_attr[i].z() = ptr[2];
                            ptr += 3;
                        }
                    }
                }
            }

            errstr = "";
            if (msg.data.vertex_attributes.count("colors")) {
                const auto& attr_arr = msg.data.vertex_attributes.at("colors");
                auto& mesh_attr = pcd->colors_;
                if (!attr_arr.CheckType({messages::TypeStr<float>(),
                                         messages::TypeStr<double>()},
                                        errstr)) {
                    errstr = "Ignoring colors. colors have wrong data type:" +
                             errstr;
                    LogInfo(errstr.c_str());
                } else if (!attr_arr.CheckShape({-1, 3}, errstr)) {
                    errstr = "Ignoring colors. colors have wrong shape:" +
                             errstr;
                    LogInfo(errstr.c_str());
                } else {
                    mesh_attr.resize(attr_arr.shape[0]);
                    if (attr_arr.type == messages::TypeStr<float>()) {
                        const float* ptr = attr_arr.Ptr<float>();
                        for (int64_t i = 0; i < attr_arr.shape[0]; ++i) {
                            mesh_attr[i].x() = ptr[0];
                            mesh_attr[i].y() = ptr[1];
                            mesh_attr[i].z() = ptr[2];
                            ptr += 3;
                        }
                    }
                    if (attr_arr.type == messages::TypeStr<double>()) {
                        const double* ptr = attr_arr.Ptr<double>();
                        for (int64_t i = 0; i < attr_arr.shape[0]; ++i) {
                            mesh_attr[i].x() = ptr[0];
                            mesh_attr[i].y() = ptr[1];
                            mesh_attr[i].z() = ptr[2];
                            ptr += 3;
                        }
                    }
                }
            }
        }
        SetGeometry(pcd, msg.path, msg.time, msg.layer);
    }

    return CreateStatusOKMsg();
}

void Receiver::SetGeometry(std::shared_ptr<geometry::Geometry3D> geom,
                           const std::string& path,
                           int time,
                           const std::string& layer) {
    std::shared_ptr<rendering::Open3DScene> scene = scene_;
    auto vis = gui_visualizer_;
    gui::Application::GetInstance().PostToMainThread(
            gui_visualizer_, [vis, geom, path, time, layer, scene]() {
              auto is_point_cloud = (geom->GetGeometryType() == geometry::Geometry::GeometryType::PointCloud);
              if (is_point_cloud && time < 1) {
                  // update UI once and clear geometry when a new geometry segment is received.
                  vis->UpdatePointcloudUI();
              }
              scene->AddGeometry("geom_" + path + layer + std::to_string(time), geom, rendering::Material());
              if (is_point_cloud) {
                  if (time < 1) {
                      // this is for update camera.
                      vis->UpdatePointcloudCamera();
                  }
                  vis->ForceRedraw();
              }
            });
}

}  // namespace visualization
}  // namespace open3d
