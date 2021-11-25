import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

images_df = pd.read_csv('../../dataset/images_test.csv', header=None)
labels_df = pd.read_csv('../../dataset/labels_test.csv', header=None)

images = np.array(images_df.to_numpy(), dtype=np.float32)
labels = np.array(labels_df.to_numpy(), dtype=int)

target_i = 0
target_img = images[target_i].reshape(128, 128)
target_lbl = labels[target_i][0]

_, ax = plt.subplots()
ax.pcolormesh(target_img)

plt.title(f'Angle: {target_lbl}')

plt.show()
