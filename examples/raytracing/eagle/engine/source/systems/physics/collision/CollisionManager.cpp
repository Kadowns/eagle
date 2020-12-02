//
// Created by Novak on 5/4/2020.
//

#include <eagle/engine/systems/physics/collision/CollisionManager.h>
#include <eagle/engine/components/physics/shapes/SphereCollider.h>
#include <eagle/engine/components/physics/shapes/BoxCollider.h>
#include <eagle/engine/components/physics/shapes/PlaneCollider.h>

EG_ENGINE_BEGIN

CollisionManager::CollisionFunctionTable CollisionManager::m_collisionFunctionTable;

void CollisionManager::configure() {
    m_collisionFunctionTable[typeid(SphereCollider)][typeid(SphereCollider)] = sphere_vs_sphere;
    m_collisionFunctionTable[typeid(SphereCollider)][typeid(PlaneCollider)] = sphere_vs_plane;
    m_collisionFunctionTable[typeid(SphereCollider)][typeid(BoxCollider)] = sphere_vs_box;

    m_collisionFunctionTable[typeid(PlaneCollider)][typeid(SphereCollider)] = plane_vs_sphere;
    m_collisionFunctionTable[typeid(PlaneCollider)][typeid(PlaneCollider)] = plane_vs_plane;
    m_collisionFunctionTable[typeid(PlaneCollider)][typeid(BoxCollider)] = plane_vs_box;

    m_collisionFunctionTable[typeid(BoxCollider)][typeid(SphereCollider)] = box_vs_sphere;
    m_collisionFunctionTable[typeid(BoxCollider)][typeid(PlaneCollider)] = box_vs_plane;
    m_collisionFunctionTable[typeid(BoxCollider)][typeid(BoxCollider)] = box_vs_box;
}

void CollisionManager::test_collision(const Collision &collision) {

    collision.contacts.clear();

    m_collisionFunctionTable[collision.colA->shape->type()][collision.colB->shape->type()](collision);

    if (collision){
        for (auto& contact : collision.contacts){
            contact.rA = contact.position - collision.rbA->current.position;
            contact.rB = contact.position - collision.rbB->current.position;
        }
        if (collision.phase & Collision::COLLIDING_BIT){
            collision.phase |= Collision::WAS_COLLIDING_BIT;
        }
        else {
            collision.phase |= Collision::COLLIDING_BIT;
        }
    }
    else {
        if (collision.phase & Collision::COLLIDING_BIT){
            collision.phase &= ~Collision::COLLIDING_BIT;
            collision.phase |= Collision::WAS_COLLIDING_BIT;
        }
        else {
            collision.phase &= ~Collision::WAS_COLLIDING_BIT;
        }
    }
}

bool CollisionManager::test_aabb(const ColliderShape::AABB &a, const ColliderShape::AABB &b) {
    if (a.max.x < b.min.x || b.max.x < a.min.x){
        return false;
    }
    if (a.max.y < b.min.y || b.max.y < a.min.y){
        return false;
    }
    if (a.max.z < b.min.z || b.max.z < a.min.z){
        return false;
    }
    return true;
}

void CollisionManager::sphere_vs_sphere(const Collision &collision) {
    Reference<SphereCollider> s1 = std::static_pointer_cast<SphereCollider>(collision.colA->shape);
    Reference<SphereCollider> s2 = std::static_pointer_cast<SphereCollider>(collision.colB->shape);

    glm::vec3 p1 = collision.rbA->current.position;
    glm::vec3 p2 = collision.rbB->current.position;

    float r = s1->radius() + s2->radius();
    float sqrDistance = glm::distance2(p1, p2);
    if (r * r < sqrDistance){
        return;
    }

    float distance = std::sqrt(sqrDistance);

    Collision::Contact contact = {};
    collision.normal = (p2 - p1) / distance;
    contact.penetration = distance - r;
    contact.position = p1 + collision.normal * s1->radius();
    contact.key = 0;

    collision.contacts.emplace_back(contact);
}

void CollisionManager::sphere_vs_plane(const Collision &collision) {
    Reference<SphereCollider> s = std::static_pointer_cast<SphereCollider>(collision.colA->shape);
    Reference<PlaneCollider> p = std::static_pointer_cast<PlaneCollider>(collision.colB->shape);

    glm::vec3 sp = collision.rbA->current.position;

    glm::mat3 pRot = glm::orientation(p->normal(), glm::vec3(0, 1, 0));

    glm::vec3 rotatedSp = sp * pRot;

    float sphereHeightFromRadius = rotatedSp.y - s->radius();
    if (sphereHeightFromRadius > -p->distance()){
        return;
    }

    Collision::Contact contact = {};
    collision.normal = -p->normal();
    contact.position = sp + (collision.normal * s->radius());
    contact.penetration = sphereHeightFromRadius - p->distance();
    contact.key = 0;

    collision.contacts.emplace_back(contact);
}

void CollisionManager::sphere_vs_box(const Collision &collision) {
    Reference<SphereCollider> sphere = std::static_pointer_cast<SphereCollider>(collision.colA->shape);
    Reference<BoxCollider> box = std::static_pointer_cast<BoxCollider>(collision.colB->shape);

    glm::vec3 spherePosition = collision.rbA->current.position;
    float sphereRadius = sphere->radius();

    glm::vec3 boxPosition = collision.rbB->current.position;
    glm::vec3 boxExtents = box->extents();
    glm::mat3 boxRotation = glm::mat3_cast(collision.rbB->current.rotation);


    glm::vec3 t = (spherePosition - boxPosition) * boxRotation;
    glm::vec3 localClosestPoint = glm::clamp(t, -boxExtents, boxExtents);
    glm::vec3 worldClosestPoint = localClosestPoint * glm::inverse(boxRotation) + boxPosition;

    float distanceToClosestPoint = glm::distance(spherePosition, worldClosestPoint);

    if (distanceToClosestPoint > sphereRadius){
        return;
    }

    Collision::Contact contact = {};
    contact.position = worldClosestPoint;
    contact.penetration = sphereRadius - distanceToClosestPoint;
    contact.key = 0;
    collision.normal = (worldClosestPoint - spherePosition) / distanceToClosestPoint;
    collision.contacts.emplace_back(contact);
}

void CollisionManager::plane_vs_sphere(const Collision &collision) {

    Collision temp = collision;
    std::swap(temp.colA, temp.colB);
    std::swap(temp.rbA, temp.rbB);
    sphere_vs_plane(temp);
    std::swap(collision.contacts, temp.contacts);
    collision.normal = -temp.normal;
}

void CollisionManager::plane_vs_plane(const Collision &collision) {

}

void CollisionManager::plane_vs_box(const Collision &collision) {

    Collision temp = collision;
    std::swap(temp.colA, temp.colB);
    std::swap(temp.rbA, temp.rbB);
    box_vs_plane(temp);
    std::swap(collision.contacts, temp.contacts);
    collision.normal = -temp.normal;
}

void CollisionManager::box_vs_sphere(const Collision &collision) {

    Collision temp = collision;
    std::swap(temp.colA, temp.colB);
    std::swap(temp.rbA, temp.rbB);
    sphere_vs_box(temp);
    std::swap(collision.contacts, temp.contacts);
    collision.normal = -temp.normal;
}

void CollisionManager::box_vs_plane(const Collision &collision) {
    Reference<BoxCollider> box = std::static_pointer_cast<BoxCollider>(collision.colA->shape);
    Reference<PlaneCollider> plane = std::static_pointer_cast<PlaneCollider>(collision.colB->shape);
    glm::mat3 planeRotation = glm::orientation(plane->normal(), glm::vec3(0, 1, 0));
    glm::mat3 boxRotation = glm::mat3_cast(collision.rbA->current.rotation);
    glm::vec3 boxPosition = collision.rbA->current.position;
}

void CollisionManager::box_vs_box(const Collision &collision) {
    Reference<BoxCollider> boxA = std::static_pointer_cast<BoxCollider>(collision.colA->shape);
    Reference<BoxCollider> boxB = std::static_pointer_cast<BoxCollider>(collision.colB->shape);

    glm::vec3 eA = boxA->extents();
    glm::vec3 eB = boxB->extents();

    Rigidbody::Transform trA = collision.rbA->current;
    Rigidbody::Transform trB = collision.rbB->current;

    glm::mat3 rotA = glm::mat3_cast(trA.rotation);
    glm::mat3 rotB = glm::mat3_cast(trB.rotation);


    // B's frame in A's space
    glm::mat3 C = glm::transpose(rotA) * rotB;

    glm::mat3 absC;


    bool parallel = false;
    const float kCosTol = 1.0e-6f;
    for (uint32_t i = 0; i < 3; i++){
        for (uint32_t j = 0; j < 3; j++){
            float val = std::abs(C[i][j]);
            absC[i][j] = val;
            if (val + kCosTol >= 1.0f){
                parallel = true;
            }
        }
    }


    glm::vec3 t = glm::transpose(rotA) * (trB.position - trA.position);


    // Query states
    float s;
    float aMax = -std::numeric_limits<float>::max();
    float bMax = -std::numeric_limits<float>::max();
    float eMax = -std::numeric_limits<float>::max();
    int32_t aAxis = ~0;
    int32_t bAxis = ~0;
    int32_t eAxis = ~0;
    glm::vec3 nA;
    glm::vec3 nB;
    glm::vec3 nE;

    // Face axis checks

    // a's x axis
    s = std::abs(t.x) - (eA.x + glm::dot(glm::row(absC, 0), eB));
    if (track_face_axis(aAxis, 0, s, aMax, nA, rotA[0])) {
        return;
    }

    // a's y axis
    s = std::abs(t.y) - (eA.y + glm::dot(glm::row(absC, 1), eB));
    if (track_face_axis(aAxis, 1, s, aMax, nA, rotA[1])) {
        return;
    }

    // a's y axis
    s = std::abs(t.z) - (eA.z + glm::dot(glm::row(absC, 2), eB));
    if (track_face_axis(aAxis, 2, s, aMax, nA, rotA[2])) {
        return;
    }

    // b's x axis
    s = std::abs(glm::dot(t, C[0])) - (eB.x + glm::dot(absC[0], eA));
    if (track_face_axis(bAxis, 3, s, bMax, nB, rotB[0])) {
        return;
    }

    // b's y axis
    s = std::abs(glm::dot(t, C[1])) - (eB.y + glm::dot(absC[1], eA));
    if (track_face_axis(bAxis, 4, s, bMax, nB, rotB[1])) {
        return;
    }

    // b's z axis
    s = std::abs(glm::dot(t, C[2])) - (eB.z + glm::dot(absC[2], eA));
    if (track_face_axis(bAxis, 5, s, bMax, nB, rotB[2])) {
        return;
    }

    if (!parallel) {

        // Edge axis checks
        float rA;
        float rB;

        // Cross( a.x, b.x )
        rA = eA.y * absC[0][2] + eA.z * absC[0][1];
        rB = eB.y * absC[2][0] + eB.z * absC[1][0];
        s = std::abs(t.z * C[0][1] - t.y * C[0][2]) - (rA + rB);
        if (track_face_edge(eAxis, 6, s, eMax, nE, glm::vec3(0.0, -C[0][2], C[0][1]))) {
            return;
        }

        // Cross( a.x, b.y )
        rA = eA.y * absC[1][2] + eA.z * absC[1][1];
        rB = eB.x * absC[2][0] + eB.z * absC[0][0];
        s = std::abs(t.z * C[1][1] - t.y * C[1][2]) - (rA + rB);
        if (track_face_edge(eAxis, 7, s, eMax, nE, glm::vec3(0.0, -C[1][2], C[1][1]))) {
            return;
        }

        // Cross( a.x, b.z )
        rA = eA.y * absC[2][2] + eA.z * absC[2][1];
        rB = eB.x * absC[1][0] + eB.y * absC[0][0];
        s = std::abs(t.z * C[2][1] - t.y * C[2][2]) - (rA + rB);
        if (track_face_edge(eAxis, 8, s, eMax, nE, glm::vec3(0.0, -C[2][2], C[2][1]))) {
            return;
        }

        // Cross( a.y, b.x )
        rA = eA.x * absC[0][2] + eA.z * absC[0][0];
        rB = eB.y * absC[2][1] + eB.z * absC[1][1];
        s = std::abs(t.x * C[0][2] - t.z * C[0][0]) - (rA + rB);
        if (track_face_edge(eAxis, 9, s, eMax, nE, glm::vec3(C[0][2], 0.0, -C[0][0]))) {
            return;
        }

        // Cross( a.y, b.y )
        rA = eA.x * absC[1][2] + eA.z * absC[1][0];
        rB = eB.x * absC[2][1] + eB.z * absC[0][1];
        s = std::abs(t.x * C[1][2] - t.z * C[1][0]) - (rA + rB);
        if (track_face_edge(eAxis, 10, s, eMax, nE, glm::vec3(C[1][2], 0.0, -C[1][0]))) {
            return;
        }

        // Cross( a.y, b.z )
        rA = eA.x * absC[2][2] + eA.z * absC[2][0];
        rB = eB.x * absC[1][1] + eB.y * absC[0][1];
        s = std::abs(t.x * C[2][2] - t.z * C[2][0]) - (rA + rB);
        if (track_face_edge(eAxis, 11, s, eMax, nE, glm::vec3(C[2][2], 0.0, -C[2][0]))) {
            return;
        }

        // Cross( a.z, b.x )
        rA = eA.x * absC[0][1] + eA.y * absC[0][0];
        rB = eB.y * absC[2][2] + eB.z * absC[1][2];
        s = std::abs(t.y * C[0][0] - t.x * C[0][1]) - (rA + rB);
        if (track_face_edge(eAxis, 12, s, eMax, nE, glm::vec3(-C[0][1], C[0][0], 0.0))) {
            return;
        }

        // Cross( a.z, b.y )
        rA = eA.x * absC[1][1] + eA.y * absC[1][0];
        rB = eB.x * absC[2][2] + eB.z * absC[0][2];
        s = std::abs(t.y * C[1][0] - t.x * C[1][1]) - (rA + rB);
        if (track_face_edge(eAxis, 13, s, eMax, nE, glm::vec3(-C[1][1], C[1][0], 0.0))) {
            return;
        }

        // Cross( a.z, b.z )
        rA = eA.x * absC[2][1] + eA.y * absC[2][0];
        rB = eB.x * absC[1][2] + eB.y * absC[0][2];
        s = std::abs(t.y * C[2][0] - t.x * C[2][1]) - (rA + rB);
        if (track_face_edge(eAxis, 14, s, eMax, nE, glm::vec3(-C[2][1], C[2][0], 0.0))) {
            return;
        }
    }

    // Artificial axis bias to improve frame coherence
    const float kRelTol = 0.95f;
    const float kAbsTol = 0.01f;
    int32_t axis;
    float sMax;
    glm::vec3 n;
    float faceMax = std::max(aMax, bMax);
    if (kRelTol * eMax > faceMax + kAbsTol) {
        axis = eAxis;
        sMax = eMax;
        n = nE;
    } else {
        if (kRelTol * bMax > aMax + kAbsTol) {
            axis = bAxis;
            sMax = bMax;
            n = nB;
        } else {
            axis = aAxis;
            sMax = aMax;
            n = nA;
        }
    }

    if (glm::dot(n, trB.position - trA.position) < 0.0f){
        n = -n;
    }

    if (axis == ~0){
        return;
    }

    if (axis < 6) {
        Rigidbody::Transform trI{};
        Rigidbody::Transform trR{};
        glm::vec3 eI;
        glm::vec3 eR;
        bool flip;

        if (axis < 3) {
            trR = trA;
            trI = trB;
            eR = eA;
            eI = eB;
            flip = false;
        } else {
            trR = trB;
            trI = trA;
            eR = eB;
            eI = eA;
            flip = true;
            n = -n;
        }

        glm::vec3 incident[4];
        compute_incident_face(trI, eI, n, incident);
        glm::mat3 basis;
        glm::vec3 e;
        compute_reference_basis(trR, eR, n, axis, basis, e);

        glm::vec3 out[8];
        float depths[8];
        uint32_t outCount;
        outCount = clip(trR.position, e, basis, incident, out, depths);

        if (outCount) {
            collision.contacts.reserve(outCount);
            collision.normal = flip ? -n : n;

            for (uint32_t i = 0; i < outCount; ++i) {
                Collision::Contact contact = {};
                contact.position = out[i];
                contact.penetration = depths[i];
//                contact.key = axis;
                collision.contacts.emplace_back(contact);
            }
        }
    }
    else {
        n = trA.rotation * n;

        if (glm::dot(n, trB.position - trA.position) < 0.0)
            n = -n;

        glm::vec3 PA, QA;
        glm::vec3 PB, QB;
        support_edge(trA, eA, n, &PA, &QA);
        support_edge(trB, eB, -n, &PB, &QB);

        glm::vec3 CA, CB;
        edges_contact(CA, CB, PA, QA, PB, QB);

        collision.normal = n;

        glm::vec3 position = (CA + CB) * 0.5f;

        Collision::Contact contact = {};
        contact.penetration = sMax;
        contact.position = position;
        contact.key = axis;
        collision.contacts.emplace_back(contact);
    }
}

bool CollisionManager::track_face_axis(int32_t &axis, int32_t axisNumber, float s, float &sMax, glm::vec3& axisNormal, const glm::vec3 &normal) {
    if (s > 0.0) {
        return true;
    }

    if (s > sMax) {
        sMax = s;
        axis = axisNumber;
        axisNormal = normal;
    }
    return false;
}

bool CollisionManager::track_face_edge(int32_t &axis, int32_t axisNumber, float s, float &sMax, glm::vec3 &axisNormal, const glm::vec3 &normal) {

    if (s > 0.0f){
        return true;
    }

    float l = 1.0f / glm::length(normal);
    s *= l;

    if (s > sMax){
        sMax = s;
        axis = axisNumber;
        axisNormal = normal * l;
    }

    return false;
}

void CollisionManager::compute_incident_face(const Rigidbody::Transform &trI, const glm::vec3 &e, glm::vec3 n, glm::vec3 *out) {
    glm::mat3 rotI = glm::mat3_cast(trI.rotation);
    n = -(glm::transpose(rotI) * n);
    glm::vec3 absN = glm::abs(n);

    if (absN.x > absN.y && absN.x > absN.z) {
        if (n.x > 0.0f) {
            out[0] = glm::vec3(e.x, e.y, -e.z);
            out[1] = glm::vec3(e.x, e.y, e.z);
            out[2] = glm::vec3(e.x, -e.y, e.z);
            out[3] = glm::vec3(e.x, -e.y, -e.z);
        } else {
            out[0] = glm::vec3(-e.x, -e.y, e.z);
            out[1] = glm::vec3(-e.x, e.y, e.z);
            out[2] = glm::vec3(-e.x, e.y, -e.z);
            out[3] = glm::vec3(-e.x, -e.y, -e.z);
        }
    } else if (absN.y > absN.x && absN.y > absN.z) {
        if (n.y > 0.0) {
            out[0] = glm::vec3(-e.x, e.y, e.z);
            out[1] = glm::vec3(e.x, e.y, e.z);
            out[2] = glm::vec3(e.x, e.y, -e.z);
            out[3] = glm::vec3(-e.x, e.y, -e.z);
        } else {
            out[0] = glm::vec3(e.x, -e.y, e.z);
            out[1] = glm::vec3(-e.x, -e.y, e.z);
            out[2] = glm::vec3(-e.x, -e.y, -e.z);
            out[3] = glm::vec3(e.x, -e.y, -e.z);
        }
    } else {
        if (n.z > 0.0) {
            out[0] = glm::vec3(-e.x, e.y, e.z);
            out[1] = glm::vec3(-e.x, -e.y, e.z);
            out[2] = glm::vec3(e.x, -e.y, e.z);
            out[3] = glm::vec3(e.x, e.y, e.z);
        } else {
            out[0] = glm::vec3(e.x, -e.y, -e.z);
            out[1] = glm::vec3(-e.x, -e.y, -e.z);
            out[2] = glm::vec3(-e.x, e.y, -e.z);
            out[3] = glm::vec3(e.x, e.y, -e.z);
        }
    }

    for (uint32_t i = 0; i < 4; ++i) {
        out[i] = (rotI * out[i] + trI.position);
    }
}

void CollisionManager::compute_reference_basis(const Rigidbody::Transform &trR, const glm::vec3 &eR,
                                                  glm::vec3 n, int32_t axis, glm::mat3 &basis,
                                                  glm::vec3 &e) {

    glm::mat3 rotR = glm::mat3_cast(trR.rotation);
    n = glm::transpose(rotR) * n;

    if ( axis >= 3 )
        axis -= 3;

    switch ( axis ) {
        case 0:
            if (n.x > 0.0) {
                e = glm::vec3(eR.y, eR.z, eR.x);
                basis[0] = rotR[1];
                basis[1] = rotR[2];
                basis[2] = rotR[0];
            }
            else {
                e = glm::vec3(eR.z, eR.y, eR.x);
                basis[0] = rotR[2];
                basis[1] = rotR[1];
                basis[2] = -rotR[0];
            }
            break;

        case 1:
            if (n.y > 0.0) {
                e = glm::vec3(eR.z, eR.x, eR.y);
                basis[0] = rotR[2];
                basis[1] = rotR[0];
                basis[2] = rotR[1];
            }
            else {
                e = glm::vec3(eR.z, eR.x, eR.y);
                basis[0] = rotR[2];
                basis[1] = -rotR[0];
                basis[2] = -rotR[1];
            }
            break;

        case 2:
            if (n.z > 0.0) {
                e = glm::vec3(eR.y, eR.x, eR.z);
                basis[0] = -rotR[1];
                basis[1] = rotR[0];
                basis[2] = rotR[2];
            }
            else {
                e = glm::vec3(eR.y, eR.x, eR.z);
                basis[0] = -rotR[1];
                basis[1] = -rotR[0];
                basis[2] = -rotR[2];
            }
            break;
    }
}

uint32_t
CollisionManager::clip(const glm::vec3 &posR, const glm::vec3 &e, const glm::mat3 &basis, glm::vec3 *incident,
                          glm::vec3 *outVerts, float *outDepths) {

    uint32_t inCount = 4;
    uint32_t outCount;
    glm::vec3 in[8];
    glm::vec3 out[8];

    for (uint32_t i = 0; i < 4; ++i){
        in[i] = glm::transpose(basis) * (incident[i] - posR);
    }

    outCount = ortographic(1.0, e.x, 0, in, inCount, out);
    if (!outCount){
        return 0;
    }

    inCount = ortographic(1.0, e.y, 1, out, outCount, in);
    if (!inCount){
        return 0;
    }

    outCount = ortographic(-1.0, e.x, 0, in, inCount, out);
    if (!outCount){
        return 0;
    }

    inCount = ortographic(-1.0, e.y, 1, out, outCount, in);

    outCount = 0;
    for (uint32_t i = 0; i < inCount; ++i) {
        float d = in[i].z - e.z;

        if (d <= 0.0) {
            outVerts[outCount] = (basis * in[i]) + posR;
            outDepths[outCount++] = d;
        }
    }

    assert(outCount <= 8);

    return outCount;
}

#define InFront( a ) \
	((a) < 0.0f)

#define Behind( a ) \
	((a) >= 0.0f)

#define On( a ) \
	((a) < 0.005f && (a) > -0.005f)

uint32_t
CollisionManager::ortographic(float sign, float e, float axis, glm::vec3 *in, uint32_t inCount, glm::vec3 *out) {
    uint32_t outCount = 0;
    glm::vec3 a = in[inCount - 1];

    for (uint32_t i = 0; i < inCount; ++i) {
        glm::vec3 b = in[i];

        float da = sign * a[axis] - e;
        float db = sign * b[axis] - e;

        glm::vec3 cv;

        // B
        if (((InFront(da) && InFront(db)) || On(da) || On(db))) {
            assert(outCount < 8);
            out[outCount++] = b;
        }

            // I
        else if (InFront(da) && Behind(db)) {
            cv = a + (b - a) * (da / (da - db));
            assert(outCount < 8);
            out[outCount++] = cv;
        }

            // I, B
        else if (Behind(da) && InFront(db)) {
            cv = a + (b - a) * (da / (da - db));
            assert(outCount < 8);
            out[outCount++] = cv;

            assert(outCount < 8);
            out[outCount++] = b;
        }

        a = b;
    }

    return outCount;
}

void CollisionManager::support_edge(const Rigidbody::Transform &tr, const glm::vec3 &e, glm::vec3 n, glm::vec3 *aOut, glm::vec3 *bOut) {

    glm::mat3 rot = glm::mat3_cast(tr.rotation);
    n = glm::transpose(rot) * n;
    glm::vec3 absN = glm::abs(n);
    glm::vec3 a, b;

    // x > y
    if (absN.x > absN.y) {
        // x > y > z
        if (absN.y > absN.z) {
            a = glm::vec3(e.x, e.y, e.z);
            b = glm::vec3(e.x, e.y, -e.z);
        }

            // x > z > y || z > x > y
        else {
            a = glm::vec3(e.x, e.y, e.z);
            b = glm::vec3(e.x, -e.y, e.z);
        }
    }

        // y > x
    else {
        // y > x > z
        if (absN.x > absN.z) {
            a = glm::vec3(e.x, e.y, e.z);
            b = glm::vec3(e.x, e.y, -e.z);
        }

            // z > y > x || y > z > x
        else {
            a = glm::vec3(e.x, e.y, e.z);
            b = glm::vec3(-e.x, e.y, e.z);
        }
    }

    float signx = n.x >= 0.0f ? 1.0f : -1.0f;
    float signy = n.y >= 0.0f ? 1.0f : -1.0f;
    float signz = n.z >= 0.0f ? 1.0f : -1.0f;

    a.x *= signx;
    a.y *= signy;
    a.z *= signz;
    b.x *= signx;
    b.y *= signy;
    b.z *= signz;

    *aOut = rot * a + tr.position;
    *bOut = rot * b + tr.position;
}

void CollisionManager::edges_contact(glm::vec3 &ca, glm::vec3 &cb, const glm::vec3 &PA, const glm::vec3 &QA,
                                        const glm::vec3 &PB, const glm::vec3 &QB) {
    glm::vec3 DA = QA - PA;
    glm::vec3 DB = QB - PB;
    glm::vec3 r = PA - PB;
    float a = glm::dot( DA, DA );
    float e = glm::dot( DB, DB );
    float f = glm::dot( DB, r );
    float c = glm::dot( DA, r );

    float b = glm::dot( DA, DB );
    float denom = a * e - b * b;

    float TA = (b * f - c * e) / denom;
    float TB = (b * TA + f) / e;

    ca = PA + DA * TA;
    cb = PB + DB * TB;
}




EG_ENGINE_END