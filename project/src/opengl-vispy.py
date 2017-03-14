#!/usr/bin/python3
# -*- coding: utf-8 -*-

from vispy import app, gloo, visuals, scene, io

vertex_shader = """
    varying vec4 color;
    void main() {
        vec4 visual_pos = vec4($position, 1);
        vec4 doc_pos = $visual_to_doc(visual_pos);
        gl_Position = $doc_to_render(doc_pos);
        
        vec4 visual_pos2 = $doc_to_visual(doc_pos + vec4(0, 0, -1, 0));
        vec4 view_direction = (visual_pos2 / visual_pos2.w) - visual_pos;
        view_direction = vec4(normalize(view_direction.xyz), 0);
        
        color = vec4(view_direction.rgb, 1);
    }
"""

fragment_shader = """
    varying vec4 color;
    void main() {
        gl_FragColor = color;
    }
"""

class MyMeshVisual(visuals.Visual):

    def __init__(self):
        visuals.Visual.__init__(self, vertex_shader, fragment_shader)

        fname = '../res/spine.obj'
        vertices, faces, normals, tex = io.read_mesh(fname)

        self._ibo = gloo.IndexBuffer(faces)

        self.shared_program.vert['position'] = gloo.VertexBuffer(vertices)
        # self.program.vert['normal'] = gloo.VertexBuffer(normals)
        self.set_gl_state('additive', cull_face=False)
        self._draw_mode = 'triangles'
        self._index_buffer = self._ibo

    def _prepare_transforms(self, view):
        # Note we use the "additive" GL blending settings so that we do not
        # have to sort the mesh triangles back-to-front before each draw.
        tr = view.transforms
        view_vert = view.view_program.vert
        view_vert['visual_to_doc'] = tr.get_transform('visual', 'document')
        view_vert['doc_to_visual'] = tr.get_transform('document', 'visual')
        view_vert['doc_to_render'] = tr.get_transform('document', 'render')


MyMesh = scene.visuals.create_visual_node(MyMeshVisual)

canvas = scene.SceneCanvas(keys='interactive', show=True)

view = canvas.central_widget.add_view()
view.camera = 'turntable'
view.camera.fov = 90
view.camera.distance = 2

mesh = MyMesh(parent=view.scene)
mesh.transform = visuals.transforms.MatrixTransform()
mesh.transform.translate([0, 0, -10])
#mesh.transform.rotate(90, (1, 0, 0))

axis = scene.visuals.XYZAxis(parent=view.scene)

if __name__ == '__main__':
    import sys
    if sys.flags.interactive != 1:
        app.run()
