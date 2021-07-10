#version 450

const int kWidth = 1600;
const int kHeight = 1200;

layout(location = 0) out vec4 outColor;

const float kInfinity = 1.0 / 0.0;

const float kAspectRatio = float(kWidth) / kHeight;
const float kViewportHeight = 2.0;
const float kViewportWidth = kAspectRatio * kViewportHeight;

const float kFocalLength = 1.0;

const vec3 kCamera = vec3(0, 0, 0);
const vec3 kHorizontal = vec3(kViewportWidth, 0, 0);
const vec3 kVertical = vec3(0, kViewportHeight, 0);
const vec3 kLowerLeftCorner = kCamera - (kHorizontal / 2 + kVertical / 2 + vec3(0, 0, kFocalLength));

const int kNumberOfSpheres = 2;

float LengthSquared(in vec3 coords) {
    return dot(coords, coords);
}
float Length(in vec3 vector) {
    return sqrt(LengthSquared(vector));
}
vec3 Normalized(in vec3 vector) {
    return vector / Length(vector);
}

struct Ray {
    vec3 origin;
    vec3 direction;  // Direction should always be normalized (Length = 1.0)
};
vec3 RayAt(in Ray ray, in float t) {
    return (ray.origin + t * ray.direction);
}

struct Sphere {
    vec3 center;
    float radius;
};
struct HitRecord {
    vec3 point;
    vec3 normal;
    float t;
};
bool SphereHit(in Sphere sphere, in Ray ray, float t_min, float t_max, inout HitRecord hit_record) {
    vec3 oc = ray.origin - sphere.center;

    float a = LengthSquared(ray.direction);
    float half_b = dot(oc, ray.direction);
    float c = LengthSquared(oc) - sphere.radius * sphere.radius;

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
    hit_record.point = RayAt(ray, hit_record.t);
    hit_record.normal = (hit_record.point - sphere.center) / sphere.radius;

    return true;
}
bool SpheresHit(in Sphere[kNumberOfSpheres] spheres, in Ray ray, float t_min, float t_max, inout HitRecord hit_record) {
    bool hit_anything = false;
    float closest_t = t_max;
    for (int i = 0; i < kNumberOfSpheres; ++i) {
        if (SphereHit(spheres[i], ray, t_min, closest_t, hit_record)) {
            hit_anything = true;
            closest_t = hit_record.t;
        }
    }

    return hit_anything;
}

vec3 processRay(in Ray ray, in Sphere[kNumberOfSpheres] world) {
    HitRecord hit_record;
    if (SpheresHit(world, ray, 0, kInfinity, hit_record)) {
        return (hit_record.normal + vec3(1, 1, 1)) / 2;
    }

    float skyCoefficient = (ray.direction.y + 1.0) / 2.0;
    return vec3(1, 1, 1) - skyCoefficient * vec3(1, 0, 0);
}

void main() {
    float x = gl_FragCoord.x / kWidth;
    float y = 1. - gl_FragCoord.y / kHeight;

    Sphere[kNumberOfSpheres] world = {
    Sphere(vec3(0, 0, -1), 0.5),
    Sphere(vec3(0, -100.5, -1), 100)
    };

    Ray r = Ray(kCamera, Normalized(kLowerLeftCorner +
                                    x * kHorizontal +
                                    y * kVertical -
                                    kCamera));

    outColor = vec4(processRay(r, world), 1.0);
}
