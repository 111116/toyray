#pragma once

#include <vector>
#include "math/vecfloat.hpp"


class Curve {
protected:
    std::vector<vec3f> controls;
public:
    explicit Curve(std::vector<vec3f> points) : controls(points) {}

    // bool intersect(const Ray &r, Hit &h, float tmin) override {
    //     return false;
    // }

    // std::vector<vec3f> &getControls() {
    //     return controls;
    // }

    virtual std::pair<vec3f, vec3f> atParam(double t) const = 0;
    // virtual void discretize(int resolution, std::vector<CurvePoint>& data) = 0;

};

class BezierCurve : public Curve {
public:
    explicit BezierCurve(const std::vector<vec3f> &points) : Curve(points) {
        if (points.size() < 4 || points.size() % 3 != 1) {
            throw "Number of control points of BezierCurve must be 3n+1!";
        }
    }

    std::pair<vec3f, vec3f> atParam(double t) const
    {
        std::vector<vec3f> p = controls;
        std::vector<vec3f> d(p.size(), 0); // dpdt
        while (p.size() > 1) {
            for (int i=0; i<p.size()-1; ++i) {
                d[i] = (1-t) * d[i] - p[i] + t * d[i+1] + p[i+1];
                p[i] = (1-t) * p[i] + t * p[i+1];
            }
            d.pop_back();
            p.pop_back();
        }
        return {p[0], d[0]};
    }

    // void discretize(int resolution, std::vector<CurvePoint>& data) override {
    //     data.clear();
    //     // fill in data vector
    //     for (int i=0; i<=resolution; ++i)
    //     {
    //         double t = (double)i / resolution;
    //         std::vector<vec3f> p = controls;
    //         std::vector<vec3f> d(p.size(), 0);
    //         while (p.size() > 1) {
    //             for (int i=0; i<p.size()-1; ++i) {
    //                 d[i] = (1-t) * d[i] - p[i] + t * d[i+1] + p[i+1];
    //                 p[i] = (1-t) * p[i] + t * p[i+1];
    //             }
    //             d.pop_back();
    //             p.pop_back();
    //         }
    //         data.push_back({p[0], d[0].normalized()});
    //     }
    // }
};


class BsplineCurve : public Curve {
public:
    BsplineCurve(const std::vector<vec3f> &points) : Curve(points) {
        if (points.size() < 4) {
            throw "Number of control points of BspineCurve must be more than 4!";
        }
    }

    std::pair<vec3f, vec3f> atParam(double t) const
    {
        const int n = controls.size()-1;
        const int k = 3;
        double tmin = (double)k/(n+k+1);
        double tmax = (double)(n+1)/(n+k+1);
        // de-normalize parameter
        t = tmin + t * (tmax - tmin);

        #define t_(i) ((double)(i)/(n+k+1))
        double B[n+k+1][k+1];
        for (int i=0; i<n+k+1; ++i)
            B[i][0] = (t_(i)<=t && t<t_(i+1))? 1: 0;
        for (int p=1; p<=k; ++p)
            for (int i=0; i+p<n+k+1; ++i)
                B[i][p] = (t-t_(i))/(t_(i+p)-t_(i))*B[i][p-1] + (t_(i+p+1)-t)/(t_(i+p+1)-t_(i+1))*B[i+1][p-1];
        vec3f res(0), der(0);
        double t0=0;
        for (int i=0; i<=n; ++i) {
            res += controls[i] * B[i][k];
            t0 += B[i][k];
            der += controls[i] * k * (B[i][k-1]/(t_(i+k)-t_(i)) - B[i+1][k-1]/(t_(i+k+1)-t_(i+1)));
        }
        #undef t_
        return {res, der};
    }

    // void discretize(int resolution, std::vector<CurvePoint>& data) override {
    //     data.clear();
    //     const int n = controls.size()-1;
    //     const int k = 3;
    //     // TODO (PA3): fill in data vector
    //     #define t_(i) ((double)(i)/(n+k+1))
    //     for (double t=t_(k); t<=t_(n+1)+1e-5; t+=1.0/resolution/(n+k+1))
    //     {
    //         double B[n+k+1][k+1];
    //         for (int i=0; i<n+k+1; ++i)
    //             B[i][0] = (t_(i)<=t && t<t_(i+1))? 1: 0;
    //         for (int p=1; p<=k; ++p)
    //             for (int i=0; i+p<n+k+1; ++i)
    //                 B[i][p] = (t-t_(i))/(t_(i+p)-t_(i))*B[i][p-1] + (t_(i+p+1)-t)/(t_(i+p+1)-t_(i+1))*B[i+1][p-1];
    //         vec3f res(0), der(0);
    //         double t0=0;
    //         for (int i=0; i<=n; ++i) {
    //             res += controls[i] * B[i][k];
    //             t0 += B[i][k];
    //             der += controls[i] * k * (B[i][k-1]/(t_(i+k)-t_(i)) - B[i+1][k-1]/(t_(i+k+1)-t_(i+1)));
    //         }
    //         data.push_back({res, der.normalized()});
    //     }
    //     #undef t_
    // }

};

