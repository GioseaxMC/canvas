class Sprite {
public:

    Texture texture;
    Vec2f position;
    float rotation;
    float scale;

    Collider collider;

    Sprite(Texture _texture, Vec2f _position={}, Collider _collider={}, float _rotation=0, float _scale=1) : texture(_texture);

    bool touching(Sprite& other);

    bool touching(Vec2f other_pos);

    void draw();
};