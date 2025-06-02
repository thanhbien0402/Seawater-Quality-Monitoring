'use client'

import { ReactNode, useContext } from 'react'
import { SelectedDeviceContext } from '../context/SelectedDeviceProvider'
import { TelemetryProvider } from '../context/TelemetryProvider'
import { getRanges } from '@/actions'

export default function MapClientWrapper({ children }: {
    children: ReactNode,
    // dataPH: any,
    // dataTUR: any,
    // dataORP: any,
    // dataEC: any
}) {
    const { selectedDevice } = useContext(SelectedDeviceContext)

    return (
        <TelemetryProvider deviceId={selectedDevice?.id.id}
        >
            {children}
        </TelemetryProvider>
    )
}