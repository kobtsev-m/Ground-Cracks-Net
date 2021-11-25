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

target_view = model(images).detach().flatten()
labels_view = labels.flatten()

mean_fault = (labels_view - target_view).mean()
print(f'{mean_fault = :.3f}')

correct = sum(abs(x - y) < 5 for x, y in zip(labels_view, target_view))
accuracy = correct / images.shape[0] * 100
print(f'{accuracy = :.1f}%')
