from scripts.renderers.base import BaseRenderer

class OptionsRenderer(BaseRenderer):
    template_name="headers/options.hpp.j2"

    def render_options(self, peripheral_options, output):
        for name, options in peripheral_options.items():
            namespace = options["namespace"]
            header_dir = output/"include"/"hal"/f"{name}"
            self.render_to_file(header_dir/"options.hpp",
                                options=options["options"],
                                namespace=namespace)
