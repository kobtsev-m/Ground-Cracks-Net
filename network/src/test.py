import torch
import numpy as np
import pandas as pd


model = torch.load('../artifacts/model.pt')
model.eval()

# Data parse
images_df = pd.read_csv('../../dataset/images_test.csv', header=None)
labels_df = pd.read_csv('../../dataset/labels_test.csv', header=None)

images = torch.tensor(images_df.to_numpy(), dtype=torch.float32)
labels = torch.tensor(labels_df.to_numpy(), dtype=torch.float32)

target_view = model(images).detach().view(1, -1).numpy()
labels_view = labels.view(1, -1).numpy()

with np.printoptions(formatter={'float': '{:.1f}'.format}):
    print(labels_view[0, :10])
    print(target_view[0, :10])
