class Sprite {
public:

    Texture texture;
    Vec2f position;
    float rotation;
    float scale;

    Collider collider;

    Sprite(Texture _texture, Vec2f _position={}, Collider _collider={}, float _rotation=0, float _scale=1) : texture(_texture) {
        position = _position;
        rotation = _rotation;
        scale = _scale;
        collider = _collider;
    }

    bool touching(Sprite& other) {
        return collider.touching(position, other.collider, other.position, scale, other.scale);
    }

    bool touching(Vec2f other_pos) {
        return collider.touching(position, other_pos, scale);
    }

    void draw() {
        texture.draw(position, scale, rotation);
    }
};