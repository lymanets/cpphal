from scripts.renderers.base import BaseRenderer

class TagsRenderer(BaseRenderer):
    template_name="headers/tags.hpp.j2"

    def render_tags(self, peripheral_options, output):
        for name, options in peripheral_options.items():
            namespace = options["namespace"]
            header_dir = output/"include"/"hal"/f"{name}"
            self.render_to_file(header_dir/"tags.hpp",
                                options=options["options"],
                                namespace=namespace)
