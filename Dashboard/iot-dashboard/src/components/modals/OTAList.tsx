'use client'
import { useContext } from 'react'
import { SelectedDeviceContext } from '../context/SelectedDeviceProvider'
import { OTAModal } from './OTAModal'
import { TelemetryProvider } from '../context/TelemetryProvider'

export function OTAList() {

    const { deviceList } = useContext(SelectedDeviceContext)
    console.log('deviceList', deviceList)

    return (
        <>
            {deviceList.map((dev) => <TelemetryProvider key={dev?.id.id} deviceId={dev?.id.id}>
                <OTAModal deviceId={dev?.id.id} isInMap={true} deviceName={dev?.name} />
            </TelemetryProvider>
            )}
        </>
    )
}
