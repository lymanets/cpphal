from scripts.renderers.base import BaseRenderer

class PeripheralHeaderRenderer(BaseRenderer):
    template_name="headers/peripheral_header.hpp.j2"

    def render_header(self, name, output):
        header_dir = output/"include"/"hal"/f"{name}"
        self.render_to_file(header_dir/f"{name}.hpp",
                            name=name)
