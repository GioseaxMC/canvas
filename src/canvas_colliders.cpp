class Collider {
public:
    enum Type {
        Null,
        Circle,
        Rectangle
    };

    struct Shape {
        Type type;

        union {
            struct {
                float radius;
            } circle;

            struct {
                float x;
                float y;
            } rectangle;
        };
    } shape;

    Collider() {
        shape.type = Type::Null;
    }

    Collider(float radius) {
        shape.type = Type::Circle;
        shape.circle.radius = radius;
    }

    Collider(float width, float height) {
        shape.type = Type::Rectangle;
        shape.rectangle.x = width;
        shape.rectangle.y = height;
    }

    bool touching(Vec2f my_pos, Collider& other, Vec2f other_pos,
                float scale, float other_scale = 1) {
        if (shape.type == Type::Null || other.shape.type == Type::Null)
            return false;

        if (shape.type == Type::Circle) {
            float radius = shape.circle.radius * scale;

            if (other.shape.type == Type::Circle) {
                float dx = my_pos.x - other_pos.x;
                float dy = my_pos.y - other_pos.y;
                float r = radius + other.shape.circle.radius * other_scale;

                return dx * dx + dy * dy <= r * r;
            }

            float half_x = other.shape.rectangle.x * other_scale / 2;
            float half_y = other.shape.rectangle.y * other_scale / 2;

            float x = std::clamp(my_pos.x, other_pos.x - half_x, other_pos.x + half_x);
            float y = std::clamp(my_pos.y, other_pos.y - half_y, other_pos.y + half_y);

            float dx = my_pos.x - x;
            float dy = my_pos.y - y;

            return dx * dx + dy * dy <= radius * radius;
        }

        if (other.shape.type == Type::Circle)
            return other.touching(other_pos, *this, my_pos, other_scale, scale);

        float dx = std::abs(my_pos.x - other_pos.x);
        float dy = std::abs(my_pos.y - other_pos.y);

        return dx <= (shape.rectangle.x * scale + other.shape.rectangle.x * other_scale) / 2 &&
            dy <= (shape.rectangle.y * scale + other.shape.rectangle.y * other_scale) / 2;
    }

    bool touching(Vec2f my_pos, Vec2f other_pos, float scale) {
        if (shape.type == Type::Null)
            return false;

        if (shape.type == Type::Circle) {
            float radius = shape.circle.radius * scale;
            float dx = my_pos.x - other_pos.x;
            float dy = my_pos.y - other_pos.y;

            return dx * dx + dy * dy <= radius * radius;
        }

        float half_x = shape.rectangle.x * scale / 2;
        float half_y = shape.rectangle.y * scale / 2;

        float dx = std::abs(my_pos.x - other_pos.x);
        float dy = std::abs(my_pos.y - other_pos.y);

        return dx <= half_x && dy <= half_y;
    }
};