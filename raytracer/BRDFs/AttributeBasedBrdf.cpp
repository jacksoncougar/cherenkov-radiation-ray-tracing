//
// Created by root on 2/3/20.
//

#include "AttributeBasedBrdf.hpp"

template<>
[[nodiscard]] float AttributeBasedBrdf<ViewDependantFunction::NearSilhouette>::D(const ShadeRec &sr,
                                                                                 const Vector3D &wo,
                                                                                 const Vector3D &wi) const {
    return std::pow(sr.normal * wo, r);
}

template<>
[[nodiscard]] float AttributeBasedBrdf<ViewDependantFunction::Highlights>::D(const ShadeRec &sr,
                                                                             const Vector3D &wo,
                                                                             const Vector3D &wi) const {
    auto wr = wi - 2 * (wi * sr.normal) * sr.normal; // reflection
    return std::pow(wo * wr, r);
}

template<>
[[nodiscard]] float AttributeBasedBrdf<ViewDependantFunction::Depth>::D(const ShadeRec &sr,
                                                                        const Vector3D &wo,
                                                                        const Vector3D &wi) const {
    return std::pow(sr.normal * wo, r);
}

