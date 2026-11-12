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

    Collider();
    Collider(float radius);
    Collider(float width, float height);

    bool touching(Vec2f my_pos, Collider& other, Vec2f other_pos, float scale, float other_scale = 1);
    bool touching(Vec2f my_pos, Vec2f other_pos, float scale);
};