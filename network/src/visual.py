import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

images_df = pd.read_csv('../../dataset/images_train.csv', header=None)
labels_df = pd.read_csv('../../dataset/labels_train.csv', header=None)

images = images_df.to_numpy(dtype=np.float32)
labels = labels_df.to_numpy(dtype=int)

target_i = 21
target_img = images[target_i].reshape(128, 128)
target_lbl = labels[target_i][0]

# _, ax = plt.subplots()

plt.pcolormesh(target_img)
plt.title(f'Angle: {target_lbl}')

plt.show()
