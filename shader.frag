#version 450

#define M_PI 3.1415926535897932384626433832795

const int kWidth = 800 * 2;
const int kHeight = 400 * 2;

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform constants {
    vec3 camera;
    float yaw;
    float pitch;
}p;

const float kInfinity = 1.0 / 0.0;
const float kEps = 1e-8;

float lengthSquared(in vec3 coords) {
    return dot(coords, coords);
}
vec3 normalized(in vec3 vector) {
    return vector / length(vector);
}
bool nearZero(in vec3 vector) {
    return (abs(vector.x) < kEps) && (abs(vector.y) < kEps) && (abs(vector.z) < kEps);
}

vec3 cameraDirection = vec3(sin(p.yaw) * cos(p.pitch), sin(p.pitch), cos(p.yaw) * cos(p.pitch));
vec3 u = normalized(cross(vec3(0, 1, 0), cameraDirection));
vec3 v = cross(cameraDirection, u);

const float kAspectRatio = float(kWidth) / kHeight;
const float kVerticalFOV = M_PI * (90.0) / 180.0;
const float kViewportHeight = 2.0 * tan(kVerticalFOV / 2);
const float kViewportWidth = kAspectRatio * kViewportHeight;

const float kFocalLength = 1.0;

vec3 horizontal = kViewportWidth * u;
vec3 vertical = kViewportHeight * v;
vec3 lowerLeftCorner = p.camera - (horizontal / 2 + vertical / 2 - kFocalLength * cameraDirection);

const int kNumberOfSpheres = 5;
const int kNumberOfAntialisingSamples = 3;
const int kMaxDepth = 5;

float r = 1.0;
float random() {
    r = fract(sin(r * dot(vec2(gl_FragCoord.x / kWidth, gl_FragCoord.y / kHeight), vec2(12.9898,78.233))) * 43758.5453123);
    return r;
}
float random(float min, float max) {
    return min + (max - min) * random();
}
vec3 randomVec3(float min, float max) {
    return vec3(random(min, max),
                random(min, max),
                random(min, max));
}
vec3 randomInHemisphere(in vec3 normal) {
    vec3 vector = randomVec3(-1.0, 1.0);
    if (dot(vector, normal) > 0.0) {
        return vector;
    }
    return -vector;
}

struct Ray {
    vec3 origin;
    vec3 direction;  // Direction should always be normalized (length = 1.0)
};
vec3 rayAt(in Ray ray, in float t) {
    return (ray.origin + t * ray.direction);
}

#define MaterialType int
#define DiffuseType int(1)
#define ReflectiveType int(2)
struct Material {
    MaterialType type;
    vec3 albedo;
    float fuzz;
};
struct Sphere {
    vec3 center;
    float radius;
    Material material;
};
struct HitRecord {
    vec3 point;
    vec3 normal;
    Material material;
    float t;
};
bool sphereHit(in Sphere sphere, in Ray ray, float t_min, float t_max, inout HitRecord hit_record) {
    if (length(ray.origin - sphere.center) < sphere.radius) {
        return false;
    }

    vec3 oc = ray.origin - sphere.center;

    float a = lengthSquared(ray.direction);
    float half_b = dot(oc, ray.direction);
    float c = lengthSquared(oc) - sphere.radius * sphere.radius;

    float discriminant = half_b * half_b - a * c;
    if (discriminant < 0) {
        return false;
    }
    float sqrt_discriminant = sqrt(discriminant);

    float root = (-half_b - sqrt_discriminant) / a;
    if (root < t_min || t_max < root) {
        root = (-half_b + sqrt_discriminant) / a;

        if (root < t_min || t_max < root) {
            return false;
        }
    }

    hit_record.t = root;
    hit_record.point = rayAt(ray, hit_record.t);
    hit_record.normal = (hit_record.point - sphere.center) / sphere.radius;
    hit_record.material = sphere.material;

    return true;
}
bool spheresHit(in Sphere[kNumberOfSpheres] spheres, in Ray ray, float t_min, float t_max, inout HitRecord hit_record) {
    bool hit_anything = false;
    float closest_t = t_max;
    for (int i = 0; i < kNumberOfSpheres; ++i) {
        if (sphereHit(spheres[i], ray, t_min, closest_t, hit_record)) {
            hit_anything = true;
            closest_t = hit_record.t;
        }
    }

    return hit_anything;
}

bool scatter(inout Ray ray, in HitRecord hit_record, inout vec3 color) {
    switch (hit_record.material.type) {
        case DiffuseType: {
            vec3 scatterDirection = hit_record.normal + randomVec3(-1.0, 1.0);
            if (nearZero(scatterDirection)) {
                scatterDirection = hit_record.normal;
            }
            ray = Ray(hit_record.point, scatterDirection);
            color = hit_record.material.albedo;
            return true;
        }
        case ReflectiveType: {
            float cos_alpha = dot(ray.direction, hit_record.normal) / (length(ray.direction) * length(hit_record.normal));
            ray = Ray(hit_record.point, normalized(ray.direction - 2 * hit_record.normal * cos_alpha + hit_record.material.fuzz * randomInHemisphere(hit_record.normal)));
            color = vec3(1, 1, 1) * 0.9;
            return true;
        }
        default: {
            return false;
        }
    }
}
vec3 processRay(Ray ray, in Sphere[kNumberOfSpheres] world) {
    vec3 color = vec3(1, 1, 1);
    HitRecord hit_record;
    int depth = 0;
    while (spheresHit(world, ray, 0.001, kInfinity, hit_record)) {
        if (depth >= kMaxDepth) {
            return vec3(0, 0, 0);
        }

        vec3 attenuation;
        if (scatter(ray, hit_record, attenuation)) {
            color *= attenuation;
        } else {
            return vec3(0, 0, 0);
        }

        ++depth;
    }

    float skyCoefficient = (ray.direction.y + 1.0) / 2.0;
    vec3 skyColor = vec3(1, 1, 1) - skyCoefficient * vec3(1, 0, 0);
    if (depth == 0) {
        return skyColor;
    } else if (hit_record.material.type == ReflectiveType) {
        return color * skyColor;
    } else {
        return color;
    }
}

void main() {
    Material Camera = Material(DiffuseType, vec3(0.0, 0.0, 0.0), 0.0);
    Material Ground = Material(DiffuseType, vec3(0.1, 0.5, 0.0), 0.0);
    Material DiffuseMaterial = Material(DiffuseType, vec3(1.0, 0.0, 0.0), 0.0);
    Material ReflectiveMaterial = Material(ReflectiveType, vec3(0.7, 0.3, 0.3), 0.025);
    Material ReflectiveFuzzedMaterial = Material(ReflectiveType, vec3(0.7, 0.3, 0.3), 1.0);

    Sphere[kNumberOfSpheres] world = {
        Sphere(p.camera,                0.25, Camera),
        Sphere(vec3( 0.0, -100.5, 1.5), 100,  Ground),
        Sphere(vec3( 0.0,    0.0, 1.5), 0.5,  DiffuseMaterial),
        Sphere(vec3(-1.0,    0.0, 1.5), 0.5,  ReflectiveMaterial),
        Sphere(vec3( 1.0,    0.0, 1.5), 0.5,  ReflectiveFuzzedMaterial),
    };

    Ray ray;
    ray.origin = p.camera;
    vec3 color = vec3(0, 0, 0);
    for (int i = 0; i < kNumberOfAntialisingSamples; ++i) {
        float x = (gl_FragCoord.x + random()) / (kWidth - 1.0);
        float y = 1.0 - (gl_FragCoord.y + random()) / (kHeight - 1.0);
        ray.direction = normalized(lowerLeftCorner +
                                   x * horizontal +
                                   y * vertical -
                                   p.camera);

        color += processRay(ray, world);
    }

    outColor = vec4(color / kNumberOfAntialisingSamples, 1.0);
}
