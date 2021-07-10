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

vec3 processRay(in Ray ray) {
    HitRecord hit_record;
    if (SphereHit(Sphere(vec3(0, 0, -1), 0.5), ray, 0, kInfinity, hit_record)) {
        return vec3(1, 0, 0);
    }

    float skyCoefficient = (ray.direction.y + 1.0) / 2.0;
    return vec3(1, 1, 1) - skyCoefficient * vec3(1, 0, 0);
}

void main() {
    float x = gl_FragCoord.x / kWidth;
    float y = 1. - gl_FragCoord.y / kHeight;

    Ray r = Ray(kCamera, Normalized(kLowerLeftCorner +
                                    x * kHorizontal +
                                    y * kVertical -
                                    kCamera));

    outColor = vec4(processRay(r), 1.0);
}
