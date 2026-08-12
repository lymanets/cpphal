from .base import BaseRenderer

class RegisterRenderer(BaseRenderer):
    template_name="register.hpp.j2"

    def render_register(self, register, namespace, output):
        self.render_to_file(output/f"{register.name.lower()}.hpp",
                            register=register,
                            namespace=namespace)
